#coding=utf-8
import sys

reload(sys)
sys.setdefaultencoding('utf-8')

def isright(command):                   #判断命令中指定账号性别一栏是否合法
    if command[1] != 'no' and command[1]!= "boy" and command[1] != "girl":
        print u"指定账号一栏填写出错，请重写"
        sys.exit()

def getuname():                         #获取账号列表
    uname=[]
    with open('user.txt','r')as f:
        for line in f:
            unamelist = []
            if line.strip()!="":
                filt = line.split('--')
                if filt[0].strip()!="" and filt[1]!="":
                    unamelist.append(filt[0].strip())
                    unamelist.append(filt[1].strip())
                    uname.append(unamelist)
                else:
                    print u"账号或密码有空，请重新检查"
                    sys.exit()
    if len(uname) != 0:
        print u"获取登陆账号成功"
    else:
        print u"读入账号出错，请在账号文件里按格式输入账号"
        sys.exit()
    return uname

def getcommand():                  #获取命令列表
    print u"正在获取命令…"
    zfcommand=[]                        #命令列表
    plcommand=[]
    dzcommand=[]
    pldzcommand=[]
    fscommand=[]
    with open('commands.txt','r') as f:
        for line in f:
            commandfile = line.split('--')
            try:                                                    #判断格式文件是否合法
                if commandfile[0].strip() == 'fs' and commandfile[1].strip() != "" and commandfile[2].strip() != "":
                    isright(commandfile)
                    fscommand.append(commandfile)
                elif commandfile[0].strip()=='zf' and commandfile[1].strip()!="" and commandfile[2].strip()!="" and commandfile[3].strip()!="" and commandfile[4].strip()!="":
                    isright(commandfile)
                    zfcommand.append(commandfile)
                elif commandfile[0].strip()=='pl' and commandfile[1].strip()!="" and commandfile[2].strip()!="" and commandfile[3].strip()!="":
                    isright(commandfile)
                    plcommand.append(commandfile)
                elif commandfile[0].strip()=='dz' and commandfile[1].strip()!="" and commandfile[2].strip()!="":
                    isright(commandfile)
                    dzcommand.append(commandfile)
                elif commandfile[0].strip()=='':                    #文件末尾空格跳过
                    pass
                else:
                    print u"命令格式出错，请重新检查"
                    sys.exit()
            except IndexError:
                print u"命令格式出错，请重新检查"
                sys.exit()
        print u"获取命令成功"
        return fscommand,zfcommand,plcommand,dzcommand