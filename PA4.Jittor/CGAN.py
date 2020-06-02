#!/usr/bin/env python
# coding: utf-8

# In[1]:


import jittor as jt
from jittor import nn
import numpy as np
import pylab as pl

get_ipython().run_line_magic('matplotlib', 'inline')

# 隐空间向量长度
latent_dim = 100
# 类别数量
n_classes = 10
# 图片大小
img_size = 32
# 图片通道数量
channels = 1
# 图片张量的形状
img_shape = (channels, img_size, img_size)


# In[2]:


class Generator(nn.Module):
    def __init__(self):
        super(Generator, self).__init__()
        self.label_emb = nn.Embedding(n_classes, n_classes)

        def block(in_feat, out_feat, normalize=True):
            layers = [nn.Linear(in_feat, out_feat)]
            if normalize:
                layers.append(nn.BatchNorm1d(out_feat, 0.8))
            layers.append(nn.LeakyReLU(0.2))
            return layers
        self.model = nn.Sequential(
            *block((latent_dim + n_classes), 128, normalize=False), 
            *block(128, 256), 
            *block(256, 512), 
            *block(512, 1024), 
            nn.Linear(1024, int(np.prod(img_shape))), 
            nn.Tanh())

    def execute(self, noise, labels):
        gen_input = jt.contrib.concat((self.label_emb(labels), noise), dim=1)
        img = self.model(gen_input)
        img = img.view((img.shape[0], *img_shape))
        return img


# In[3]:


class Discriminator(nn.Module):
    def __init__(self):
        super(Discriminator, self).__init__()
        self.label_embedding = nn.Embedding(n_classes, n_classes)
        self.model = nn.Sequential(
            nn.Linear((n_classes + int(np.prod(img_shape))), 512), 
            nn.LeakyReLU(0.2), 
            nn.Linear(512, 512), 
            nn.Dropout(0.4), 
            nn.LeakyReLU(0.2), 
            nn.Linear(512, 512), 
            nn.Dropout(0.4), 
            nn.LeakyReLU(0.2), 
            nn.Linear(512, 1))

    def execute(self, img, labels):
        d_in = jt.contrib.concat((img.view((img.shape[0], (- 1))), self.label_embedding(labels)), dim=1)
        validity = self.model(d_in)
        return validity


# In[4]:


get_ipython().system('wget https://cg.cs.tsinghua.edu.cn/jittor/assets/build/generator_last.pkl')
get_ipython().system('wget https://cg.cs.tsinghua.edu.cn/jittor/assets/build/discriminator_last.pkl')


# In[5]:


# 定义模型
generator = Generator()
discriminator = Discriminator()
generator.eval()
discriminator.eval()

# 加载参数
generator.load('./generator_last.pkl')
discriminator.load('./discriminator_last.pkl')

# 定义一串数字
number = "2017011071"
n_row = len(number)
z = jt.array(np.random.normal(0, 1, (n_row, latent_dim))).float32().stop_grad()
labels = jt.array(np.array([int(number[num]) for num in range(n_row)])).float32().stop_grad()
gen_imgs = generator(z,labels)

pl.imshow(gen_imgs.data.transpose((1,2,0,3))[0].reshape((gen_imgs.shape[2], -1)))
