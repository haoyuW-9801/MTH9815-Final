#!/usr/bin/env python
# coding: utf-8

# In[8]:


import pandas as pd
import numpy as np


# In[24]:


def combine_to_decimal(a,b):
    c = int(256 * b)
    remain = c // 8
    res = c % 8
    s = str(a) + '.'
    if remain < 10:
        s += '0' + str(remain)
    else:
        s += str(remain)
    if res == 4:
        s += '+'
    else:
        s += str(res)
    return s

def turn_to_decimal(f):
    a = int(f)
    b = f - a
    return combine_to_decimal(a,b)

result = []
bond_id = ["912828ZX1","912828ZY9","912828ZW3","912828ZV5","912828ZZ6","912810ST6","912810SS8"]
for j in range(7):
    for i in range(1000000):
        a = 99 + (i % 512)/256
        b = a + (1 + i % 128)/128
        result.append([bond_id[j],turn_to_decimal(a),turn_to_decimal(b)])
result = pd.DataFrame(result)


# In[25]:


result.to_csv('prices.txt',sep='\t',index=False)


# In[11]:


result1 = []
book_id = ["TRSY1","TRSY2","TRSY3"]
quantities = [1000000,2000000,3000000,4000000,5000000]
for j in range(7):
    for i in range(10):
        book = book_id[i%3];
        trade_id = bond_id[j] + str(i)
        if i%2 ==0:
            price = 99
            side = "BUY"
        else:
            price = 100
            side = "SELL"
        quantity = quantities[i%5]
        result1.append([bond_id[j],trade_id,turn_to_decimal(price),book,quantity,side])
result1 = pd.DataFrame(result1)
result1.to_csv('trades.txt',sep='\t',index=False)


# In[28]:


result2 = []
book_id = ["TRSY1","TRSY2","TRSY3"]
quantities = [1000000,2000000,3000000,4000000,5000000]
for j in range(7):
    for i in range(1000000):
        mid = 99 + (i % 512)/256
        spread = 1/128 * (1 + i % 4)
        for k in range(5):
            bid = mid - (spread + k/128) /2
            offer = mid + (spread + k/128) / 2
            if(offer - bid <= 1/128):
                print("algo")
            quantity = quantities[k]
            result2.append([bond_id[j],trade_id,turn_to_decimal(bid),trade_id,turn_to_decimal(offer),quantity])
result2 = pd.DataFrame(result2)
result2.to_csv('marketdata.txt',sep='\t',index=False)


# In[27]:


result3 = []
book_id = ["TRSY1","TRSY2","TRSY3"]
quantities = [1000000,2000000,3000000,4000000,5000000]
for j in range(7):
    for i in range(10):
        inquiryid = str(j * 10 + i)
        productid = bond_id[j]
        quantity = quantities[i%5]
        state = "RECEIVED"
        if i%2 ==0:
            price = 99
            side = "BUY"
        else:
            price = 100
            side = "SELL"
        result3.append([inquiryid,productid,side,quantity,turn_to_decimal(price),state])
result3 = pd.DataFrame(result3)
result3.to_csv('inquiry.txt',sep='\t',index=False) 


# In[ ]:




