#-*-coding;utf-8-*-

import requests
from lxml import etree
from multiprocessing.dummy import Pool

cook={"Cookie":"SINAGLOBAL=4961246474810.266.1483631294321; wb_publish_fist100_6098867923=1; wvr=6; UOR=,,login.sina.com.cn; WBStorage=194a5e7d191964cc|undefined; ULV=1483773139386:10:10:10:2380339183176.0103.1483773139354:1483773081072; un=17015678428; YF-Ugrow-G0=5b31332af1361e117ff29bb32e4d8439; SCF=AnPNgspGaOaPwQgBTUOk1NYsZHXn9YgKy1SCw8Ff6aPk-5l6EDeqrXJVuJnzX5SN6S9sso8CRyAFjnJGQIZPRro.; SUB=_2A251dOVZDeRxGeBO4loZ9inFyT-IHXVWAFGRrDV8PUNbmtBeLWTGkW9yMTMeT9_LMNnPj3pEOdbx2IldiQ..; SUBP=0033WrSXqPxfM725Ws9jqgMF55529P9D9WFyhEhI1RVTMGCkqbKOlsIc5JpX5KMhUgL.Foq71KnRSoM4eoe2dJLoI0qLxKqL1h5L1-BLxK-L1KzL12qLxKnL1hzLB-qLxKnL1-eLB.zLxKqLBonLB-qLxK-L12zLB.qt; SUHB=0U1JXQwo4BSq-f; ALF=1515309193; SSOLoginState=1483773194; YF-V5-G0=73b58b9e32dedf309da5103c77c3af4f"}
url='http://weibo.com/u/6098867923/home'
html=requests.get(url,cookies=cook).content
print html
purl='http://weibo.com/aj/v6/like/add?ajwvr=6'
pdata='location=page_100505_home&version=mini&qid=heart&mid=4025510483301217&loc=profileName'
phtml=requests.post(url,cookies=cook,data=pdata).content
print phtml

