import sys
import random
from collections import deque
import numpy as np
from keras.layers import Dense, InputLayer
from keras.models import Sequential
from keras.optimizers import Adam
from keras import backend as K


class ActorSunhoAgent:
    def __init__(self, name, state_size, action_size, training):
        self.state_size = state_size
        self.action_size = action_size
        self.value_size = 1
        
        self.discount_factor = 0.99
        self.actor_lr = 0.001
        self.critic_lr = 0.005
        self.name=name

        self.actor = self.build_actor()
        self.critic = self.build_critic()

        
        if training==1:
            self.actor.load_weights(self.name+"actor.h5")
            self.critic.load_weights(self.name+"critic.h5")

    def build_actor(self):
        actor = Sequential()
        actor.add(InputLayer(input_shape=[self.state_size]))
        actor.add(Dense(self.state_size*3, activation='relu',
                        kernel_initializer='he_uniform'))
        actor.add(Dense(self.state_size*2, activation='relu',
                        kernel_initializer='he_uniform'))
        actor.add(Dense(self.action_size, activation='softmax',
        kernel_initializer='he_uniform'))
        actor.summary()
        actor.compile(loss="categorical_crossentropy",
                      optimizer=Adam(lr=self.actor_lr))
        return actor

    def build_critic(self):
        critic = Sequential()
        critic.add(InputLayer(input_shape=[self.state_size]))
        critic.add(Dense(self.state_size*4, activation='relu',
                         kernel_initializer='he_uniform'))
        critic.add(Dense(self.state_size*3, activation='relu',
                         kernel_initializer='he_uniform'))
        critic.add(Dense(self.state_size*2, activation='relu',
                         kernel_initializer='he_uniform'))
        critic.add(Dense(self.value_size, activation='linear',
                      kernel_initializer='he_uniform'))
        critic.summary()
        critic.compile(loss="mse", optimizer=Adam(lr=self.critic_lr))
        return critic

    def get_action(self, state):
        state=np.reshape(state,[1,self.state_size])
        policy = self.actor.predict(state, batch_size=1).flatten()
        return np.random.choice(self.action_size, 1, p=policy)[0]
    def set_name(self,name):
        self.name=name

    def train_model(self, state, action, reward, next_state, done):
        state=np.reshape(state,[1,self.state_size])
        next_state=np.reshape(next_state,[1,self.state_size])
        target = np.zeros((1, self.value_size))
        advantages = np.zeros((1, self.action_size))
        
        value = self.critic.predict(state)[0]
        next_value = self.critic.predict(next_state)[0]
        
        if done:
            advantages[0][action] = reward - value
            target[0][0] = reward
        else:
            advantages[0][action] = reward + self.discount_factor * (
                                                                     next_value - value)
            target[0][0] = reward + self.discount_factor * next_value
        
        self.actor.fit(state, advantages, epochs=1, verbose=0)
        self.critic.fit(state, target, epochs=1, verbose=0)
    def save(self):
        self.actor.save_weights(self.name+"actor.h5")
        self.critic.save_weights(self.name+"critic.h5")

