
Postfix V2.6.6 安装说明

软件用途：作为SMTP Server

一、软件安装

1、安装方式
Yum安装

2、安装文件
postfixInstall.sh	postfix安装                

3、安装目录
/usr/sbin

执行文件: /usr/sbin/postfix

二、配置文件
1、文件目录
/etc/postfix
/etc/sasl2

2、主配置文件
/etc/postfix/main.cf
/etc/sasl2/smtpd.conf


三、控制命令
1、postfix
启动 : service postfix start
关闭 : service postfix stop
重启 : service postfix restart

状态查询 ：service postfix status

控制文件目录：/etc/rc.d/init.d/postfix

2、saslauthd
启动 : service saslauthd start
关闭 : service saslauthd stop
重启 : service saslauthd restart

状态查询 ：service saslauthd status

控制文件目录：/etc/rc.d/init.d/saslauthd


附：
1、查看postfix的版本
postconf mail_version

2、查看postfix支持扩展
postconf -a
postconf -m

3、查看postfix端口状态
netstat -ntlp | grep 25

4、添加postfix用户
直接使用系统用户
使用useradd命令添加，然后再使用passwd来设置密码

5、域名解析(例)
A记录
smtp.doitphp.net 用于smtp server (MTA)
pop.doithp.net	用于pop server   (MUA)
imap.doitphp.net 用于imap server (MUA)
mail.doitphp.net 用于web管理控制页的访问

MX记录解析
@	doitphp.net	192.168.1.100

SPF解析
@	doitphp.net	v=spf1 ip4:123.123.123.123 ~all
或
@	doitphp.net	v=spf1 a mx ~all


注：
  postfix与cyrus-sasl在CentOS V6.5已集成
  配置文件中子目录smtptest中为smtp server的测试脚本