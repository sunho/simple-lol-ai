import numpy as np
import os
import tensorflow as tf
import math
import random
from collections import deque
class SunhoAgent:
    def __init__(self, state_size, num_actions,name, training):
        self.sess = tf.Session()
        self.num_actions = num_actions 
        self.state_size = state_size
        self.name=name
        self.gamma=0.975
        self.k=1
        self.learning_rate=0.0001
        self.define_graph()
        self.batch_size = 160
        self.memory = deque(maxlen=200000)
        self.saver = tf.train.Saver()
        self.epsilon = 0.5
        self.epsilon_decay = 0.999999
        self.epsilon_min = 0.05
        model_exists = False
        if training==1:
            self.saver.restore(self.sess, "./"+name+".ckpt")
            print("loaded")
        else:
            self.sess.run(tf.global_variables_initializer())
        
        #set up new writer
        self.summary_writer = tf.summary.FileWriter("logs"+name, self.sess.graph)
    
    
    def fully_connected_layer(self, inputs, input_size, output_size, name='fc_layer'):
        w = tf.Variable(tf.truncated_normal(shape=[input_size, output_size], stddev=0.1), name=("%s/w" % name))
        b = tf.Variable(tf.truncated_normal(shape=[output_size], stddev=0.1,), name=("%s/b" % name))
        outputs = tf.matmul(inputs, w) + b
        self.layer_collection.register_fully_connected(params=(w,b), inputs=inputs, outputs=outputs)
        return outputs
    
    
    def define_graph(self):
        self.global_step = tf.Variable(0, name="global_step", trainable=False)
        self.x_batch = tf.placeholder(dtype=tf.float32, shape=[None, self.state_size])
        self.actions_taken = tf.placeholder(dtype=tf.int32, shape=[None])
        self.actor_labels = tf.placeholder(dtype=tf.float32, shape=[None])
        self.critic_labels = tf.placeholder(dtype=tf.float32, shape=[None])
        
        self.layer_collection = tf.contrib.kfac.layer_collection.LayerCollection()

        fc_layer = self.fully_connected_layer(self.x_batch,self.state_size, 64, 'fc_layer1')
        fc_layer = tf.nn.relu(fc_layer)
        fc_layer = self.fully_connected_layer(fc_layer,64,32, 'fc_layer2')
        fc_layer = tf.nn.relu(fc_layer)
        fc_layer = self.fully_connected_layer(fc_layer,32,24, 'fc_layer2')
        fc_layer = tf.nn.relu(fc_layer)
        #policy output layer
        self.policy_logits = self.fully_connected_layer(fc_layer, 24, self.num_actions, 'policy_logits')
        self.policy_probs = tf.nn.softmax(self.policy_logits)
        
        #value output layer
        self.value_preds = self.fully_connected_layer(fc_layer, 24, 1, 'value_fc_layer')
        self.value_preds = tf.squeeze(self.value_preds)
        
        self.layer_collection.register_categorical_predictive_distribution(self.policy_logits, seed=1234)
        self.layer_collection.register_normal_predictive_distribution(self.value_preds, var=1, seed=1234)
        
        self.actor_loss = tf.reduce_mean(tf.nn.sparse_softmax_cross_entropy_with_logits(logits=self.policy_logits, labels=self.actions_taken) * self.actor_labels)
        self.critic_loss = tf.losses.huber_loss(self.critic_labels,self.value_preds)
        self.entropy_regularization = tf.reduce_mean(self.calculate_entropy(self.policy_logits))
        self.actor_loss = self.actor_loss - 0.01 * self.entropy_regularization
        
        self.total_loss = self.actor_loss + 0.5 * self.critic_loss
        
        optimizer = tf.contrib.kfac.optimizer.KfacOptimizer(self.learning_rate,
        cov_ema_decay=0.95, damping=0.001,
        layer_collection=self.layer_collection, momentum=0.9)
            
        self.train_op = optimizer.minimize(self.total_loss, global_step=self.global_step)
        
        #summaries
        self.a_loss_summary = tf.summary.scalar("actor_loss", self.actor_loss)
        self.c_loss_summary = tf.summary.scalar("critic_loss", self.critic_loss)
        
        self.ep_reward = tf.placeholder(tf.float32)
        self.ep_reward_summary = tf.summary.scalar("episode_reward", self.ep_reward)

    def calculate_entropy(self, logits):
        a0 = logits - tf.reduce_max(logits, 1, keep_dims=True)
        ea0 = tf.exp(a0)
        z0 = tf.reduce_sum(ea0, 1, keep_dims=True)
        if z0==0:
            z0=1.0e-10
        p0 = ea0 / z0
        return tf.reduce_sum(p0 * (tf.log(z0) - a0), axis=1)
    def train_model2(self, s_batch, a_batch, r_batch, s_next_batch, terminal_batch):
        v_s = self.sess.run([self.value_preds], feed_dict={self.x_batch: s_batch})
        v_s_next = self.sess.run([self.value_preds], feed_dict={self.x_batch: s_next_batch})
        v_s_next = np.array(v_s_next)
        v_s_next *= terminal_batch #mask out preds for terminal states
        #create labels
        critic_return_labels = r_batch + v_s_next * self.gamma #estiated k-step return
        actor_advantage_labels = critic_return_labels - v_s #estiated k-step return - v_s
        #reshape to remove extra dim
        critic_return_labels = np.reshape(critic_return_labels, [-1]) #turn into row vec
        actor_advantage_labels = np.reshape(actor_advantage_labels, [-1]) #turn into row vec
        sess_args = [self.global_step, self.a_loss_summary, self.c_loss_summary, self.train_op]
        feed_dict = {self.x_batch: s_batch,
        self.actor_labels: actor_advantage_labels,
        self.critic_labels: critic_return_labels,
        self.actions_taken: a_batch}
        step, a_summary, c_summary, _ = self.sess.run(sess_args, feed_dict=feed_dict)
        if (step - 1) % 100 == 0:
            self.summary_writer.add_summary(a_summary, global_step=step)
            self.summary_writer.add_summary(c_summary, global_step=step)
            self.summary_writer.flush()
        
        if (step - 1) % 10000 == 0:
            self.saver.save(self.sess, "./"+self.name+".ckpt", global_step=step)
        return step

    def train_model(self,s,a,r,s_,t):
        s= np.reshape(s,[1,self.state_size])
        s_= np.reshape(s_,[1,self.state_size])
        self.memory.append((s,a,r,s_,t))
        if len(self.memory) > self.batch_size:
            mini_batch = random.sample(self.memory, self.batch_size)

            s = np.zeros((self.batch_size, self.state_size))
            s_ = np.zeros((self.batch_size, self.state_size))
            action, reward, done = [], [], []
        
            for i in range(self.batch_size):
                s[i] = mini_batch[i][0]
                action.append(mini_batch[i][1])
                reward.append(mini_batch[i][2])
                s_[i] = mini_batch[i][3]
                done.append(mini_batch[i][4])
            self.train_model2(s,action,reward,s_,done)
    

    # TODO later: increase temp of softmax over time?
    def get_action_softmax(self, state):
        if self.epsilon > self.epsilon_min:
            self.epsilon *= self.epsilon_decay
        if np.random.rand() <= self.epsilon:
            return random.randrange(self.num_actions)
        else:
            state= np.reshape(state,[1,self.state_size])
            feed_dict = {self.x_batch: state}
            policy_probs = self.sess.run(self.policy_probs, feed_dict=feed_dict)
            policy_probs = np.squeeze(policy_probs)
            return np.random.choice(len(policy_probs), p=policy_probs)

    def get_action(self, state):
        state= np.reshape(state,[1,self.state_size])
        feed_dict = {self.x_batch: state}
        policy_logits = self.sess.run(self.policy_logits, feed_dict=feed_dict)
        policy_logits = np.squeeze(policy_logits)
        noise = np.random.rand(*policy_logits.shape)
        print(np.argmax(policy_logits - np.log(-np.log(noise))))
        return np.argmax(policy_logits - np.log(-np.log(noise)))
    
    def write_ep_reward_summary(self, ep_reward, steps):
        summary = self.sess.run(self.ep_reward_summary,
        feed_dict={self.ep_reward: ep_reward})
        self.summary_writer.add_summary(summary, global_step=steps)
