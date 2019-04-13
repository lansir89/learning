#coding=utf-8
from selenium import webdriver
from selenium.webdriver.common.desired_capabilities import DesiredCapabilities
from selenium.webdriver.common.keys import Keys
from module.file import *
from module.weibo import *
import sys
import os.path

reload(sys)
sys.setdefaultencoding('utf-8')
chromeOptions = webdriver.ChromeOptions()
prefs = {"profile.managed_default_content_settings.images": 2,                  #禁用图片
         "profile.default_content_setting_values.notifications": 2}             #禁用通知
chromeOptions.add_experimental_option("prefs", prefs)
weiboobj = webdriver.Chrome(chrome_options=chromeOptions)
weiboobj.get('http://weibo.com/login.php')

uname=getuname()                                        #用户账号列表列表,如[[user1,pwd1],[user2,pwd2]]
while 1:
    myfs,myzf,mypl,mydz=getcommand()        #命令列表，前4个返回值是列表的列表，形如[[a,b,c],[a,d]]
    lmyfs=len(myfs)
    lmyzf=len(myzf)
    lmypl=len(mypl)
    lmydz=len(mydz)
    if lmyfs==0 and lmyzf==0 and lmypl==0 and lmydz==0:
        print u"命令为空，休眠1分钟"
        sleep(60)
    else:
        if lmyfs!=0:
            fsiter=iter(myfs)
        if lmyzf!=0:
            zfiter=iter(myzf)
        if lmypl!=0:
            pliter=iter(mypl)
        if lmydz!=0:
            dziter=iter(mydz)
        print lmyfs,u'条发送微博命令，',lmyzf,u'条转发命令，',lmypl,u'条评论命令,',lmydz,u'条点赞命令，'

        for i in range(len(uname)):
            user=uname[i][0]
            pwd=uname[i][1]
            usercookiepath=('cookies/%s.txt')%(user)                    #账号cookie
            if os.path.exists(usercookiepath)==1:                       #存在cookie文件则直接cookie登陆
                cookieslogin(weiboobj,user)
            else:
                login(weiboobj,user,pwd)                       #登陆账号
            if lmyfs!=0:
                fscommand=next(fsiter)
                fsweibo(weiboobj,fscommand)                  #发送微博
                lmyfs-=1
            elif lmypl!=0:
                plcommand = next(pliter)
                plweibo(weiboobj,plcommand)
                lmypl-=1
            elif lmyzf!=0:
                zfcommand = next(zfiter)
                zfweibo(weiboobj,zfcommand)
                lmyzf-=1
            elif lmydz!=0:
                dzcommand = next(dziter)
                dzweibo(weiboobj,dzcommand)
                sjzf(weiboobj)
                lmydz-=1
            else:
                sjzf(weiboobj)
            savecookies(weiboobj,user)                      #更新cookie
            print u"更换账号"
            sleep(1)

sys.exit()