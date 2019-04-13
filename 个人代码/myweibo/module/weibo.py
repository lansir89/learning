#coding=utf-8
import requests
from selenium import webdriver
from selenium.webdriver.common.desired_capabilities import DesiredCapabilities
from selenium.webdriver.common.keys import Keys
from time import sleep
import sys,time
import re
import random
import requests

reload(sys)
sys.setdefaultencoding('utf-8')

def waittoclick(obj,string):                            #等js解释器解释完成并点击按键
    try:
        obj.find_element_by_xpath(string).click()
    except:
        pass

def getuid(pagesource):                                 #获取用户uid，用于获取cookie
    find = re.compile("\$CONFIG\[\'uid\'\]='(.*)'")
    result = find.findall(pagesource)  # 获取pagesource中所有匹配find的字符串
    for x in result:  # 输出
        if x !="":
            return x
setfilecon="".join([setfilecon[:keywords_tmp.start()+29],djkeywords,setfilecon[keywords_tmp.end()-2:]])
def waittime(obj,string1,string2,string3):                   #判断指定界面是否加载成功并等待登陆跳转
    waitsleep=1
    while waitsleep:
        if obj.title == string1.decode('utf-8'):
            print string2.decode('utf-8')
            waitsleep=0
        else:
            if string1.decode('utf-8')=="我的首页 微博-随时随地发现新鲜事" and obj.title =="微博-随时随地发现新鲜事":     #if语句专为登陆验证码设计
                yzm=obj.find_element_by_xpath("//div[@id='pl_login_form']/div/div[3]/div[3]/div/input").text
                print yzm
                if len(yzm)!=5:
                    pass
                else:
                    waittoclick(obj, "//div[@id='pl_login_form']/div/div[3]/div[6]/a")
            waitsleep=waitsleep+1
            sleep(1)
            if waitsleep==60:
                print string3.decode('utf-8')
                sys.exit()

def savecookies(obj,username):
    cookies=obj.get_cookies()
    pagesource = obj.page_source
    uid = getuid(pagesource)
    file = ('cookies/%s.%s') % (username, "txt")
    print file
    cookiesname=["ALF","SCF","SINAGLOBAL","SUBP","SUHB","ULV","un","wb_g_upvideo_","wvr","login_sid_t","YF-Ugrow-G0","YF-V5-G0","Apache","SUB","SSOLoginState","YF-Page-G0"]
    with open(file, "w+") as f:
        f.write(str(uid))
        f.write('>>>')
        for i in cookies:
            if i["name"] in cookiesname:
                f.write(i["name"]+"="+i["value"])
                f.write(",")
    f.close()

#返回一个列表，列表的内容是字典，形如[{name,myuser},{pwd,mypwd}]
def getcookies(username):
    file = ('cookies/%s.%s') % (username, "txt")
    cookielist=[]
    with open(file,"r") as f:
        mycookies=f.read()
        #for i in mycookies:
        cookiestemp1 = mycookies.strip().split(">>>")
        mycookieschar=cookiestemp1[1]
        myuid=cookiestemp1[0]
        cookiestemp2=mycookieschar.strip().split(",")
        for i in cookiestemp2:
            if i!="":
                cookielistdict = {}
                cookiesplittmp=i.split("=")
                floattype=1
                if cookiesplittmp[1] != "":
                    try:
                        floattype=type(eval(cookietemp[1]))
                    except:
                        pass
                    if cookiesplittmp[0]=="wb_g_upvideo_":
                        upvideo="wb_g_upvideo_"+myuid
                        if  cookiesplittmp[1].isdigit():
                            cookielistdict["name"] = upvideo
                            cookielistdict["value"] = int(cookiesplittmp[1])
                        else:
                            cookielistdict["name"] = cookiesplittmp[0]
                            cookielistdict["value"] = cookiesplittmp[1]
                    elif cookiesplittmp[1].isdigit():
                        cookielistdict["name"] = cookiesplittmp[0]
                        cookielistdict["value"] = int(cookiesplittmp[1])
                    elif floattype==float:
                        cookielistdict["name"] = cookiesplittmp[0]
                        cookielistdict["value"] = float(cookiesplittmp[1])
                    else:
                        cookielistdict["name"] = cookiesplittmp[0]
                        cookielistdict["value"] = cookiesplittmp[1]
            cookielist.append(cookielistdict)
        return cookielist

def getmaxplweibo():
    #首先找到最大评论数量的索引值maxindex，然后将所有的url和其索引值找出来，比maxindex是的最接近maxindex的就是要找的值
    with open("page.html","r+") as f:
        pagesource=f.read()
        find = re.compile(r"&#xe608;<\\/em><em>(\d+)<\\/em><\\/span><\\/span><\\/span><\\/a>\\r\\n")
        result = find.findall(pagesource)
        resultlist=[]
        for i in result:
            resultlist.append(int(i))               #评论数量列表
        maxindex=max(resultlist)                      #最大评论数量
    findstring=r'&#xe608;<\/em><em>%d<\/em><\/span><\/span><\/span><\/a>\r\n'%(maxindex)
    plindex=pagesource.find(findstring)               #最大评论数量所在的索引
    maxindexurl=[]
    urlindex=[]
    find = re.compile(r'\>\\r\\n                                    \<a target=\\\"_blank\\\" href=\\\"\\/(.*?)\" title=\\')
    result = find.findall(pagesource)               #地址列表
    for i in result:
        urlindex.append(pagesource.find(i))
    for i in xrange(len(urlindex)):                 #根据最大评论数量所在的偏移值找到url
        if urlindex[i]>plindex:
            maxindexurl=result[i-1].replace("\/","/")
            maxindexurl=result[i-1].replace("\\","")
            break
    return maxindexurl

def login(obj,username,password):
    obj.delete_all_cookies()
    obj.get('http://weibo.com/login.php')
    waittime(obj,"微博-随时随地发现新鲜事","获取登陆网址成功","网络出错，请稍等")
    obj.find_element_by_id("loginname").send_keys(username.decode('utf-8'))
    obj.find_element_by_name("password").send_keys(password.decode('utf-8'))
    sleep(2)
    waittoclick(obj,"//div[@id='pl_login_form']/div/div[3]/div[6]/a")
    waittime(obj,"我的首页 微博-随时随地发现新鲜事","登陆成功","登陆出错")

    savecookies(obj,username)

def cookieslogin(obj,username):
    obj.delete_all_cookies()
    #obj = webdriver.PhantomJS(service_args=['--load-images=no'])  # 不加载图片
    cookielist=getcookies(username)
    for i in cookielist:
        obj.add_cookie(i)
    obj.get('http://weibo.com/login.php')
    sleep(5000)

def fsweibo(obj,fs):
    try:
        obj.find_element_by_xpath("//div[@id='v6_pl_content_publishertop']/div/div[2]/textarea").clear()
        obj.find_element_by_xpath("//div[@id='v6_pl_content_publishertop']/div/div[2]/textarea").send_keys(fs[2].strip().decode('utf-8'))
        obj.find_element_by_xpath("//div[@id='v6_pl_content_publishertop']/div/div[3]/div[1]/a").click()
    except:
        pass
    sleep(2)
    print u"发送成功"
    return obj

def zfweibo(obj,zf):
    try:
        obj.get(zf[4].strip())
        print u"获取转发地址成功"
        #if random.uniform(10, 20)>15:
        if random.randint(10,20)>15:
            obj.find_element_by_xpath("//div[@id='Pl_Official_WeiboDetail__74']/div/div/div/div[2]/div/ul/li[4]/a/span/span/span/em[1]").click()
            print u"点赞成功"
        obj.find_element_by_xpath("//div[@id='Pl_Official_WeiboDetail__74']/div/div/div/div[2]/div/ul/li[2]/a/span/span/span/em[1]").click()
        obj.find_element_by_xpath("//div[@id='Pl_Official_WeiboDetail__74']/div/div/div/div[5]/div/div[3]/div/div/div/div/div/div[1]/textarea").clear()
        obj.find_element_by_xpath("//div[@id='Pl_Official_WeiboDetail__74']/div/div/div/div[5]/div/div[3]/div/div/div/div/div/div[1]/textarea").send_keys(zf[2].decode('utf-8'))
        if zf[3].strip()=='yes':
            obj.find_element_by_xpath("//div[@id='Pl_Official_WeiboDetail__74']/div/div/div/div[5]/div/div[3]/div/div/div/div/div/div[2]/div[2]/ul/li/label/span").click()
        obj.find_element_by_xpath("//div[@id='Pl_Official_WeiboDetail__74']/div/div/div/div[5]/div/div[3]/div/div/div/div/div/div[2]/div[1]/a").click()
    except:
        pass
    print u"转发成功"
    return obj

def plweibo(obj,pl):
    try:
        obj.get(pl[3].strip())
        print u"获取评论地址成功"
        obj.find_element_by_xpath("//div[@id='Pl_Official_WeiboDetail__74']/div/div/div/div[4]/div/div[2]/div[2]/div[1]/textarea").clear()
        obj.find_element_by_xpath("//div[@id='Pl_Official_WeiboDetail__74']/div/div/div/div[4]/div/div[2]/div[2]/div[1]/textarea").send_keys(pl[2].decode('utf-8'))
        obj.find_element_by_xpath("//div[@id='Pl_Official_WeiboDetail__74']/div/div/div/div[4]/div/div[2]/div[2]/div[2]/div[1]/a/em").click()
        if random.randint(10,20)>15:
            try:
                obj.find_element_by_xpath("//div[@id='Pl_Official_WeiboDetail__74']/div/div/div/div[2]/div/ul/li[4]/a/span/span/span/em[1]").click()
                print u"点赞成功"
            except:
                pass
    except:
        pass
    print u"评论成功"
    return obj

def dzweibo(obj,dz):
    try:
        obj.get(dz[2].strip())
        print u"获取点赞地址成功"
        obj.find_element_by_xpath("//div[@id='Pl_Official_WeiboDetail__74']/div/div/div/div[2]/div/ul/li[4]/a/span/span/span/em[1]").click()
        print u"点赞成功"
    except:
        pass
    return obj

def sjzf(obj):
    maxurl=getmaxplweibo()
    url=("http://weibo.com/%s")%(maxurl)
    print url
    obj.get(url)
    try:
        if random.randint(10, 20) > 15:
            obj.find_element_by_xpath("//div[@id='Pl_Official_WeiboDetail__74']/div/div/div/div[2]/div/ul/li[4]/a/span/span/span/em[1]").click()
            print u"点赞成功"
        obj.find_element_by_xpath("//div[@id='Pl_Official_WeiboDetail__74']/div/div/div/div[2]/div/ul/li[2]/a/span/span/span/em[1]").click()
        obj.find_element_by_xpath("//div[@id='Pl_Official_WeiboDetail__74']/div/div/div/div[5]/div/div[3]/div/div/div/div/div/div[1]/textarea").clear()
        obj.find_element_by_xpath("//div[@id='Pl_Official_WeiboDetail__74']/div/div/div/div[5]/div/div[3]/div/div/div/div/div/div[1]/textarea").send_keys()
        obj.find_element_by_xpath("//div[@id='Pl_Official_WeiboDetail__74']/div/div/div/div[5]/div/div[3]/div/div/div/div/div/div[2]/div[1]/a").click()
    except:
        pass
    print u"转发成功"
    return obj

