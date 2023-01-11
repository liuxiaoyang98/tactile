#!/usr/bin/env python
# coding: utf-8

# # Import

# In[1]:


import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
import os


# # functions

# In[2]:


def exdata(data):
    ret=[]
    for i in data.split("\n"):
        if not (i==''):
            if not (i[0]=='E'or i[0]=='S'):
                ret.append(i[2:])
    if len(ret[-1])<12:
        del ret[-1]
    if len(ret)%45!=0:
        end=9*int(len(ret)/9)
        ret=ret[:end]
    return ret


# In[3]:


def dataloader(file):
    if not os.path.exists(file):
        print("File Not Exists!")
        return
    files=open(file).read()
    files=exdata(files)
    ret=[int(j) for i in files for j in i.split(",")]
    ret=np.array(ret).reshape([int(len(ret)/45),9,5])
    return ret


# In[4]:


def getPres(x,y,s,t="list",plot=False):
    ret=[]
    for i in range(0,len(s)):
        ret.append(s[i][x][y])
    if plot!=False:
        pd.DataFrame(ret).plot()
        return 
    if t=="pd":
        return pd.DataFrame(ret,dtype="int")
    else:
        return ret


# In[5]:


def means(data):
    ret=np.empty([9,5],dtype='int')
    for i in range(9):
        for j in range(5):
            ret[i][j]=sum(getPres(i,j,data))/len(data)
    return ret


# In[6]:


def nor(data,mean):#source,mean
    for i in range(len(data)):
        for x in range(9):
            for y in range(5):
                if(data[i][x][y]-mean[x][y]<0):
                    data[i][x][y]=0
                else:
                    data[i][x][y]=data[i][x][y]-mean[x][y]


# # getmean

# In[60]:


t1airp1=dataloader("dataset/t1airp1")
t1airp2=dataloader("dataset/t1airp1")
pd.DataFrame(means(t1airp1)).to_csv("meanp1.csv")
pd.DataFrame(means(t1airp2)).to_csv("meanp2.csv")


# In[7]:


mean1=[[int(j) for j in i.split(",")[1:]] for i in open("meanp1.csv").read().split("\n")[1:-1] ]
mean2=[[int(j) for j in i.split(",")[1:]] for i in open("meanp2.csv").read().split("\n")[1:-1] ]


# # data pre-process

# In[8]:


dataname="t3walking"


# In[9]:


namep1=dataname+"p1"
namep2=dataname+"p2"
namemks=dataname+"marks"


# In[17]:


#读取
datap1=dataloader(f"dataset/{namep1}")
nor(datap1,mean1)
datap2=dataloader(f"dataset/{namep2}")
nor(datap2,mean2)

if len(datap1)>len(datap2):
    datap1=datap1[:len(datap2)]
else:
    datap2=datap2[:len(datap1)]


# In[41]:


marks=open(f"dataset/{namemks}").read()

marks=marks.split("\n")
marks=marks[1:-1]

for i in range(len(marks)):
    marks[i]=int(marks[i])/1000
print(marks)


# In[42]:


timestamp=[]
for i in open(f"dataset/{namep1}").read().split("\n"):
    if i[:3]=="ESP":
        temp=(i.split(" ")[1][:-1].split(":"))
        timestamp.append(int(temp[0])*3600+int(temp[1])*60+float(temp[2]))
for i in range(len(timestamp)):
    timestamp[i]=int(timestamp[i]*50)


# In[20]:


#画出来看看
getPres(7,2,datap1,t="pd").plot()
smp1=getPres(7,2,datap1)
getPres(7,2,datap2,t="pd").plot()
smp2=getPres(7,2,datap2)


# In[43]:


marks=[int(i*50) for i in marks]


# In[44]:


#整体看看
larger=len(smp1) if len(smp1)>len(smp2) else len(smp2)
x = np.arange(larger)
y = np.array(smp1)
z = np.array(smp2)
m = np.array(marks)
xm=np.zeros(len(marks),dtype="int")
plt.figure(figsize=(50, 6))
plt.plot(x,y)
plt.plot(x,z)
plt.plot(m,xm,"o")
for i in range(len(marks)):
    plt.text(m[i], xm[i], str(i), ha='center', va='bottom', fontsize=20)
    print(i,end=":")
    print(marks[i],end=" ")
    
plt.show()


# In[46]:


#删除没用的标记
dels=[20,21,42,43]
def delmarks(marks,dels):
    ret=[]
    for i in dels:
        marks[i]=0
    for i in marks:
        if not i==0:
            ret.append(i)
    return ret
marks=delmarks(marks,dels)


# In[48]:


#整体看看
larger=len(smp1) if len(smp1)>len(smp2) else len(smp2)
x = np.arange(larger)
y = np.array(smp1)
z = np.array(smp2)
m = np.array(marks)
xm=np.zeros(len(marks),dtype="int")
plt.figure(figsize=(50, 6))
plt.plot(x,y)
plt.plot(x,z)
plt.plot(m,xm,"o")
for i in range(len(marks)):
    plt.text(m[i], xm[i], str(i), ha='center', va='bottom', fontsize=20)
    print(i,end=":")
    print(marks[i],end=" ")
    
plt.show()


# In[49]:


#获得样品
def getsamples(smp1,marks):
    passed_marks=0
    samples=[]
    in_ranks=False
    temp=[]
    for i in range(len(smp1)):
        if(smp1[i])>marks[passed_marks]:
            in_ranks=True
            temp.append(i)
        if in_ranks==True and smp1[i] > marks[passed_marks]+249:
            in_ranks=False
            passed_marks=passed_marks+1
            samples.append(temp)
            temp=[]
        if(passed_marks==len(marks)):
            break
    return samples


# In[50]:


#获得采样数据（索引值）
samples=getsamples(timestamp,marks)
print(np.array(samples).shape)


# In[77]:


#分割样品
smp1name="walk"
smp2name="updown"
sample1=samples[0:19]
sample2=samples[20:39]


# In[96]:


#获得采样数据（具体数值）
def getsampledata(datap1,datap2,smp_index):
    sample1=np.array([[datap1[j] for j in smp_index[i]] for i in range(len(smp_index))])
    sample2=np.array([[datap2[j] for j in smp_index[i]] for i in range(len(smp_index))])

    newshape=np.zeros([len(smp_index),250,9,10],dtype="int")
    for i in range(len(newshape)):
        for j in range(len(newshape[i])):
            for h in range(9):
                newshape[i][j][h]=np.append(sample1[i][j][h],sample2[i][j][h])
    print(newshape.shape)
    return newshape


# In[97]:


sample1=getsampledata(datap1,datap2,sample1)
sample2=getsampledata(datap1,datap2,sample2)


# In[98]:


date="1-10"
tester="lxy"
smp1name="walk"
smp2name="updown"
path="dataset/"


# In[106]:


import cv2
from PIL import Image
#path="../res3d/3D-ResNets-PyTorch/data/kinetics_videos/jpg/"
def savedata(newshape,smpname,date,tester):
    cls=f"{date}_{smpname}"
    name=f"{tester}_{smpname}"
    for i in range(len(newshape)):
        times=str(i)
        if i%100==i:
            times="0"+times
        if i%10==i:
            times="0"+times

        if not os.path.exists(f"{path}{cls}/{name}-{times}/"):
            os.makedirs(f"{path}{cls}/{name}-{times}/")
        for j in range(len(newshape[i])):
            conv=(newshape[i][j]/4095)*255# 变换为0-255的灰度值
            im = Image.fromarray(conv)
            im = im.convert('L')  # 这样才能转为灰度图，如果是彩色图则改L为‘RGB’
            num=str(j)
            if j%100==j:
                num="0"+num
            if j%10==j:
                num="0"+num

            im.save(f'{path}{cls}/{name}-{times}/{num}.jpg')


# In[107]:


savedata(newshape,smp1name,date,tester)
savedata(newshape,smp2name,date,tester)


# In[48]:


root_path="/home/tensorflow-3.n/notebook/liuxiaoyang/insore-experiment/res3d/3D-ResNets-PyTorch/data"
video_path="/home/tensorflow-3.n/notebook/liuxiaoyang/insore-experiment/res3d/3D-ResNets-PyTorch/data/kinetics_videos/jpg"
annotation_path="/home/tensorflow-3.n/notebook/liuxiaoyang/insore-experiment/res3d/3D-ResNets-PyTorch/data"

print(f"python main.py --root_path {root_path} --video_path {video_path} --annotation_path kinetics.json --result_path results --dataset kinetics --model_depth 10 --n_classes 1 --no_train false --no_val false --inference --output_topk 1 --inference_batch_size 1")


# In[ ]:




