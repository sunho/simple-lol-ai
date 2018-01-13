import numpy as np
import random
import tensorflow as tf

import numpy as np
import random
from collections import deque


class ReplayMemoryFast:
	""" Simple queue for storing and sampling of minibatches.
		This implementation has been optimized for speed by pre-allocating the buffer memory and 
		by allowing the same element to be non-unique in the sampled minibatch. In practice, however, this 
		will never happen (minibatch size ~32-128 out of 10-100-1000k memory size).
	"""
	def __init__(self, memory_size, minibatch_size):
		self.memory_size = memory_size # max number of samples to store
		self.minibatch_size = minibatch_size

		self.experience = [None]*self.memory_size  #deque(maxlen = self.memory_size)
		self.current_index = 0
		self.size = 0

	def store(self, observation, action, reward, newobservation, is_terminal):
		self.experience[self.current_index] = (observation, action, reward, newobservation, is_terminal)
		self.current_index += 1
		self.size = min(self.size+1, self.memory_size)
		if self.current_index >= self.memory_size:
			self.current_index -= self.memory_size

	def sample(self):
		""" Samples a minibatch of minibatch_size size. """
		if self.size <  self.minibatch_size:
			return []

		samples_index  = np.floor(np.random.random((self.minibatch_size,))*self.size)

		samples		= [self.experience[int(i)] for i in samples_index]

		return samples

import numpy as np
import random
import tensorflow as tf



class QNetwork(object):
	"""
	Base class for QNetworks. 
	"""
	def __init__(self, input_size, output_size, name):
		self.name = name

	def weight_variable(self, shape, fanin=0):
		if fanin==0:
			initial = tf.truncated_normal(shape, stddev=0.01)
		else:
			mod_init = 1.0 / math.sqrt(fanin)
			initial = tf.random_uniform( shape , minval=-mod_init, maxval=mod_init)

		return tf.Variable(initial)

	def bias_variable(self, shape, fanin=0):
		if fanin==0:
			initial = tf.constant(0.01, shape=shape)
		else:
			mod_init = 1.0 / math.sqrt(fanin)
			initial = tf.random_uniform( shape , minval=-mod_init, maxval=mod_init)

		return tf.Variable(initial)


	def variables(self):
		return tf.get_collection(tf.GraphKeys.TRAINABLE_VARIABLES, self.name)


	def copy_to(self, dst_net):
		"""
		mn = ModelNetwork(2, 3, 0, "actor")
		mn_target = ModelNetwork(2, 3, 0, "target_actor")
		s=tf.InteractiveSession()
		s.run( tf.initialize_all_variables() )
		mn.copy_to(mn_target)
		"""
		v1 = self.variables()
		v2 = dst_net.variables()

		for i in range(len(v1)):
			v2[i].assign( v1[i] ).eval()


	def print_num_of_parameters(self):
		list_vars = self.variables()
		total_parameters = 0
		for variable in list_vars:
			# shape is an array of tf.Dimension
			shape = variable.get_shape()
			variable_parametes = 1
			for dim in shape:
				variable_parametes *= dim.value
			total_parameters += variable_parametes
		print '# of parameters in network ',self.name,': ',total_parameters,'  ->  ',np.round(float(total_parameters)/1000000.0, 2),'M'






class QNetworkNIPS(QNetwork):
	"""
	QNetwork used in ``Playing Atari with Deep Reinforcement Learning'', [Mnih et al., 2013].
	It's a Convolutional Neural Network with the following specs:
		L1: 16 8x8 filters with stride 4  +  RELU
		L2: 32 4x4 filters with stride 2  +  RELU
		L3: 256 unit Fully-Connected layer  +  RELU
		L4: [output_size] output units, Fully-Connected
	"""
	def __init__(self, input_size, output_size, name):
		self.name = name

		self.input_size = input_size
		self.output_size = output_size

		with tf.variable_scope(self.name):
			## 16 8x8 filters, stride=4;   32 4x4 filters, stride=2;   256 fc layer,   output layer

			self.W_conv1 = self.weight_variable([8, 8, 4, 16]) # 32 8x8 filters over 4 channels (frames)
			self.B_conv1 = self.bias_variable([16])
			self.stride1 = 4

			self.W_conv2 = self.weight_variable([4, 4, 16, 32])
			self.B_conv2 = self.bias_variable([32])
			self.stride2 = 2

			# FC layer
			self.W_fc4 = self.weight_variable([9*9*32, 256])#, fanin=11*11*32)
			self.B_fc4 = self.bias_variable([256])#, fanin=11*11*32)

			# FC layer
			self.W_fc5 = self.weight_variable([256, self.output_size])#, fanin=256)
			self.B_fc5 = self.bias_variable([self.output_size])#, fanin=256)


		# Print number of parameters in the network
		self.print_num_of_parameters()


	def __call__(self, input_tensor):
		if type(input_tensor) == list:
			input_tensor = tf.concat(1, input_tensor)

		with tf.variable_scope(self.name):
			# input_tensor is (84, 84, 4)

			self.h_conv1 = tf.nn.relu( tf.nn.conv2d(input_tensor, self.W_conv1, strides=[1, self.stride1, self.stride1, 1], padding='VALID') + self.B_conv1 )
			# max pooling:  self.h_pool1 = tf.nn.max_pool(self.h_conv1, ksize=[1, 2, 2, 1], strides=[1, 2, 2, 1], padding='SAME')

			self.h_conv2 = tf.nn.relu( tf.nn.conv2d(self.h_conv1, self.W_conv2, strides=[1, self.stride2, self.stride2, 1], padding='VALID') + self.B_conv2 )

			#self.h_conv3 = tf.nn.relu( tf.nn.conv2d(self.h_conv2, self.W_conv3, strides=[1, self.stride3, self.stride3, 1], padding='SAME') + self.B_conv3 )

			self.h_conv2_flat = tf.reshape(self.h_conv2, [-1, 9*9*32])

			self.h_fc4 = tf.nn.relu(tf.matmul(self.h_conv2_flat, self.W_fc4) + self.B_fc4)

			self.h_fc5 = tf.identity(tf.matmul(self.h_fc4, self.W_fc5) + self.B_fc5)


		return self.h_fc5






class QNetworkNature(QNetwork):
	"""
	QNetwork used in ``Human-level control through deep reinforcement learning'', [Mnih et al., 2015].
	It's a Convolutional Neural Network with the following specs:
		L1: 32 8x8 filters with stride 4  +  RELU
		L2: 64 4x4 filters with stride 2  +  RELU
		L3: 64 3x3 fitlers with stride 1  +  RELU
		L4: 512 unit Fully-Connected layer  +  RELU
		L5: [output_size] output units, Fully-Connected
	"""
	def __init__(self, input_size, output_size, name):
		self.name = name

		self.input_size = input_size
		self.output_size = output_size

		with tf.variable_scope(self.name):
			## 32 8x8 filters, stride=4;   64 4x4 filters, stride=2;   64 3x3, stride 1;   512 fc layer,   output layer

			self.W_conv1 = self.weight_variable([8, 8, 4, 32]) # 32 8x8 filters over 4 channels (frames)
			self.B_conv1 = self.bias_variable([32])
			self.stride1 = 4

			self.W_conv2 = self.weight_variable([4, 4, 32, 64])
			self.B_conv2 = self.bias_variable([64])
			self.stride2 = 2

			self.W_conv3 = self.weight_variable([3, 3, 64, 64])
			self.B_conv3 = self.bias_variable([64])
			self.stride3 = 1


			# FC layer
			self.W_fc4 = self.weight_variable([7*7*64, 512])#, fanin=11*11*32)
			self.B_fc4 = self.bias_variable([512])#, fanin=11*11*32)

			# FC layer
			self.W_fc5 = self.weight_variable([512, self.output_size])#, fanin=256)
			self.B_fc5 = self.bias_variable([self.output_size])#, fanin=256)


		# Print number of parameters in the network
		self.print_num_of_parameters()


	def __call__(self, input_tensor):
		if type(input_tensor) == list:
			input_tensor = tf.concat(1, input_tensor)

		with tf.variable_scope(self.name):
			# input_tensor is (84, 84, 4)

			self.h_conv1 = tf.nn.relu( tf.nn.conv2d(input_tensor, self.W_conv1, strides=[1, self.stride1, self.stride1, 1], padding='VALID') + self.B_conv1 )

			self.h_conv2 = tf.nn.relu( tf.nn.conv2d(self.h_conv1, self.W_conv2, strides=[1, self.stride2, self.stride2, 1], padding='VALID') + self.B_conv2 )

			self.h_conv3 = tf.nn.relu( tf.nn.conv2d(self.h_conv2, self.W_conv3, strides=[1, self.stride3, self.stride3, 1], padding='VALID') + self.B_conv3 )

			self.h_conv3_flat = tf.reshape(self.h_conv3, [-1, 7*7*64])

			self.h_fc4 = tf.nn.relu(tf.matmul(self.h_conv3_flat, self.W_fc4) + self.B_fc4)

			self.h_fc5 = tf.identity(tf.matmul(self.h_fc4, self.W_fc5) + self.B_fc5)


		return self.h_fc5











class QNetworkDueling(QNetwork):
	"""
	QNetwork used in ``Human-level control through deep reinforcement learning'', [Mnih et al., 2015].
	It's a Convolutional Neural Network with the following specs:
		L1: 32 8x8 filters with stride 4  +  RELU
		L2: 64 4x4 filters with stride 2  +  RELU
		L3: 64 3x3 fitlers with stride 1  +  RELU
		L4a: 512 unit Fully-Connected layer  +  RELU
		L4b: 512 unit Fully-Connected layer  +  RELU
		L5a: 1 unit FC + RELU (State Value)
		L5b: #actions FC + RELU (Advantage Value)
		L6: Aggregate V(s)+A(s,a)
	"""
	def __init__(self, input_size, output_size, name):
		self.name = name

		self.input_size = input_size
		self.output_size = output_size

		with tf.variable_scope(self.name):
			self.W_conv1 = self.weight_variable([8, 8, 4, 32]) # 32 8x8 filters over 4 channels (frames)
			self.B_conv1 = self.bias_variable([32])
			self.stride1 = 4

			self.W_conv2 = self.weight_variable([4, 4, 32, 64])
			self.B_conv2 = self.bias_variable([64])
			self.stride2 = 2

			self.W_conv3 = self.weight_variable([3, 3, 64, 64])
			self.B_conv3 = self.bias_variable([64])
			self.stride3 = 1


			# FC layer
			self.W_fc4a = self.weight_variable([7*7*64, 512])#, fanin=11*11*32)
			self.B_fc4a = self.bias_variable([512])#, fanin=11*11*32)

			self.W_fc4b = self.weight_variable([7*7*64, 512])#, fanin=11*11*32)
			self.B_fc4b = self.bias_variable([512])#, fanin=11*11*32)

			# FC layer
			self.W_fc5a = self.weight_variable([512, 1])#, fanin=256)
			self.B_fc5a = self.bias_variable([1])#, fanin=256)

			self.W_fc5b = self.weight_variable([512, self.output_size])#, fanin=256)
			self.B_fc5b = self.bias_variable([self.output_size])#, fanin=256)



		# Print number of parameters in the network
		self.print_num_of_parameters()


	def __call__(self, input_tensor):
		if type(input_tensor) == list:
			input_tensor = tf.concat(1, input_tensor)

		with tf.variable_scope(self.name):
			# input_tensor is (84, 84, 4)

			self.h_conv1 = tf.nn.relu( tf.nn.conv2d(input_tensor, self.W_conv1, strides=[1, self.stride1, self.stride1, 1], padding='VALID') + self.B_conv1 )

			self.h_conv2 = tf.nn.relu( tf.nn.conv2d(self.h_conv1, self.W_conv2, strides=[1, self.stride2, self.stride2, 1], padding='VALID') + self.B_conv2 )

			self.h_conv3 = tf.nn.relu( tf.nn.conv2d(self.h_conv2, self.W_conv3, strides=[1, self.stride3, self.stride3, 1], padding='VALID') + self.B_conv3 )

			self.h_conv3_flat = tf.reshape(self.h_conv3, [-1, 7*7*64])

			self.h_fc4a = tf.nn.relu(tf.matmul(self.h_conv3_flat, self.W_fc4a) + self.B_fc4a)
			self.h_fc4b = tf.nn.relu(tf.matmul(self.h_conv3_flat, self.W_fc4b) + self.B_fc4b)

			self.h_fc5a_value     = tf.identity(tf.matmul(self.h_fc4a, self.W_fc5a) + self.B_fc5a)
			self.h_fc5b_advantage = tf.identity(tf.matmul(self.h_fc4b, self.W_fc5b) + self.B_fc5b)

			self.h_fc6 = self.h_fc5a_value + ( self.h_fc5b_advantage - tf.reduce_mean(self.h_fc5b_advantage, reduction_indices=[1,], keep_dims=True) )


		return self.h_fc6

class DQN(object):
	"""
	Implementation of a DQN agent.
	reward_clipping: any negative number disables rewards clipping. Positive numbers mean that the rewards will be clipped to be in [-reward_clipping, reward_clipping]
	"""
	def __init__(self, state_size,
					   action_size,
					   name,
					   training,
					   exploration_period = 100000,
					   minibatch_size = 160,
					   discount_factor = 0.99,
					   experience_replay_buffer = 100000,
					   target_qnet_update_frequency = 30000,
					   initial_exploration_epsilon = 1.0,
					   final_exploration_epsilon = 0.1,
					   reward_clipping = 1.0,
					   DoubleDQN = True):
		self.session = tf.Session()
		# Setup the parameters, data structures and networks
		self.state_size = state_size
		self.action_size = action_size

		self.DoubleDQN = DoubleDQN

		self.session = session
		self.exploration_period = float(exploration_period)
		self.minibatch_size = minibatch_size
		self.discount_factor = tf.constant(discount_factor)
		self.experience_replay_buffer = experience_replay_buffer
		self.summary_writer = summary_writer
		self.reward_clipping = reward_clipping

		self.target_qnet_update_frequency = target_qnet_update_frequency

		self.initial_exploration_epsilon = initial_exploration_epsilon
		self.final_exploration_epsilon = final_exploration_epsilon


		self.qnet = QNetworkDueling(self.state_size, self.action_size, "qnet")
		self.target_qnet = QNetworkDueling(self.state_size, self.action_size, "target_qnet")

		self.qnet_optimizer = tf.train.RMSPropOptimizer(learning_rate=0.00025, decay=0.99, epsilon=0.01) 


		self.experience_replay = ReplayMemoryFast(self.experience_replay_buffer, self.minibatch_size)

		self.num_training_steps = 0


		# Setup the computational graph
		self.create_graph()
		self.saver = tf.train.Saver()
		model_exists = False
        if training==1:
            self.saver.restore(self.sess, "./"+name+".ckpt")
            print("loaded")
        else:
            self.session.run(tf.global_variables_initializer())
		self.summary_writer=tf.train.SummaryWriter("log"+name, self.sess.graph)

	@staticmethod
	def copy_to_target_network(source_network, target_network):
		target_network_update = []
		for v_source, v_target in zip(source_network.variables(), target_network.variables()):
			# this is equivalent to target = source
			update_op = v_target.assign(v_source)
			target_network_update.append(update_op)
		return tf.group(*target_network_update)


	def create_graph(self):
		# Pick action given state ->   action = argmax( qnet(state) )
		with tf.name_scope("pick_action"):
			self.state = tf.placeholder(tf.float32, (None,)+self.state_size , name="state")

			self.q_values = tf.identity(self.qnet(self.state) , name="q_values")
			self.predicted_actions = tf.argmax(self.q_values, dimension=1 , name="predicted_actions")

			tf.histogram_summary("Q values", tf.reduce_mean(tf.reduce_max(self.q_values, 1))) # save max q-values to track learning


		# Predict target future reward: r  +  gamma * max_a'[ Q'(s') ]
		with tf.name_scope("estimating_future_rewards"):
			# DQN vs DoubleDQN (DDQN)
			# In DQN the target is          y_i^DQN  = r + gamma * max_a' Q_target(next_state, a')
			# In DoubleDQN it's changed to  y_i^DDQN = r + gamma * Q_target(next_state, argmax_a' Q(next_state, a') )
			# In practice, we use the actual QNet (non target) to select the action for the next state, but then use its Q value estimated using the target network
			self.next_state = tf.placeholder(tf.float32, (None,)+self.state_size , name="next_state")
			self.next_state_mask = tf.placeholder(tf.float32, (None,) , name="next_state_mask") # 0 for terminal states
			self.rewards = tf.placeholder(tf.float32, (None,) , name="rewards")

			self.next_q_values_targetqnet = tf.stop_gradient(self.target_qnet(self.next_state), name="next_q_values_targetqnet")

			if self.DoubleDQN:
				# DoubleDQN
				print "Double DQN"
				self.next_q_values_qnet = tf.stop_gradient(self.qnet(self.next_state), name="next_q_values_qnet")
				self.next_selected_actions = tf.argmax(self.next_q_values_qnet, dimension=1)
				self.next_selected_actions_onehot = tf.one_hot(indices=self.next_selected_actions, depth=self.action_size)

				self.next_max_q_values = tf.stop_gradient( tf.reduce_sum( tf.mul( self.next_q_values_targetqnet, self.next_selected_actions_onehot ) , reduction_indices=[1,] ) * self.next_state_mask )



			else:
				# DQN
				print "Regular DQN"
				self.next_max_q_values = tf.reduce_max(self.next_q_values_targetqnet, reduction_indices=[1,]) * self.next_state_mask

			self.target_q_values = self.rewards + self.discount_factor*self.next_max_q_values


		# Gradient descent
		with tf.name_scope("optimization_step"):
			self.action_mask = tf.placeholder(tf.float32, (None, self.action_size) , name="action_mask") #action that was selected
			self.y = tf.reduce_sum( self.q_values * self.action_mask , reduction_indices=[1,])

			## ERROR CLIPPING AS IN NATURE'S PAPER
			self.error = tf.abs(self.y - self.target_q_values)
			quadratic_part = tf.clip_by_value(self.error, 0.0, 1.0)
			linear_part = self.error - quadratic_part
			self.loss = tf.reduce_mean( 0.5*tf.square(quadratic_part) + linear_part )

			qnet_gradients = self.qnet_optimizer.compute_gradients(self.loss, self.qnet.variables())
			for i, (grad, var) in enumerate(qnet_gradients):
				if grad is not None:
					qnet_gradients[i] = (tf.clip_by_norm(grad, 10), var)
			self.qnet_optimize = self.qnet_optimizer.apply_gradients(qnet_gradients)


		with tf.name_scope("target_network_update"):
			self.hard_copy_to_target = DQN.copy_to_target_network(self.qnet, self.target_qnet)

		self.summarize = tf.merge_all_summaries()



	def store(self, state, action, reward, next_state, is_terminal):
		# rewards clipping
		if self.reward_clipping > 0.0:
			reward = np.clip(reward, -self.reward_clipping, self.reward_clipping)

		self.experience_replay.store(state, action, reward, next_state, is_terminal)



	def action(self, state, training = False):
		"""
		If `training', compute the epsilon-greedy parameter epsilon according to the defined exploration_period, initial_epsilon and final_epsilon.
		If not `training', use a fixed testing epsilon=0.05
		"""
		if self.num_training_steps > self.exploration_period:
			epsilon = self.final_exploration_epsilon
		else:
			epsilon =  self.initial_exploration_epsilon - float(self.num_training_steps) * (self.initial_exploration_epsilon - self.final_exploration_epsilon) / self.exploration_period

		if not training:
			epsilon = 0.05

		# Execute a random action with probability epsilon, or follow the QNet policy with probability 1-epsilon.
		if random.random() <= epsilon:
			action = random.randint(0, self.action_size-1)
		else:
			action = self.session.run(self.predicted_actions, {self.state:[state] } )[0]

		return action



	def train(self):
		# Copy the QNetwork weights to the Target QNetwork.
		if self.num_training_steps == 0:
			print "Training starts..."
			self.qnet.copy_to(self.target_qnet)


		# Sample experience from replay memory
		minibatch = self.experience_replay.sample()
		if len(minibatch)==0:
			return


		# Build the bach states
		batch_states = np.asarray( [d[0] for d in minibatch] )

		actions = [d[1] for d in minibatch]
		batch_actions = np.zeros( (self.minibatch_size, self.action_size) )
		for i in xrange(self.minibatch_size):
			batch_actions[i, actions[i]] = 1

		batch_rewards = np.asarray( [d[2] for d in minibatch] )
		batch_newstates = np.asarray( [d[3] for d in minibatch] )

		batch_newstates_mask = np.asarray( [not d[4] for d in minibatch] )


		# Perform training
		scores, _, = self.session.run([self.q_values, self.qnet_optimize],
									  { self.state: batch_states,
										self.next_state: batch_newstates,
										self.next_state_mask: batch_newstates_mask,
										self.rewards: batch_rewards,
										self.action_mask: batch_actions} )


		if self.num_training_steps % self.target_qnet_update_frequency == 0:
			# Hard update (copy) of the weights every # iterations
			self.session.run( self.hard_copy_to_target )


			# Write logs
			print 'mean maxQ in minibatch: ',np.mean(np.max(scores,1))

			str_ = self.session.run(self.summarize, { self.state: batch_states,
										self.next_state: batch_newstates,
										self.next_state_mask: batch_newstates_mask,
										self.rewards: batch_rewards,
										self.action_mask: batch_actions})
			self.summary_writer.add_summary(str_, self.num_training_steps)


		self.num_training_steps += 1