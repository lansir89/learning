#!/bin/bash

# Check if user is root
if [ $(id -u) != "0" ]; then
    printf "Error: You must be root to run this script!\n"
    exit 1
fi

printf "\n"
printf "============================\n"
printf " CentOS V6.5 init setup	    \n"
printf " copyright: www.doitphp.com \n"
printf "============================\n"
printf "\n\n"

#show system params
#cpu
printf "\nCPU:\n"
cat /proc/cpuinfo

printf "\nCPU Core Num:\n"
cat /proc/cpuinfo | grep "cpu cores"

printf "\nMemery (unit:M):\n"
free -m

printf "\nMemery (unit:G):\n"
free -g

printf "\nDisk:\n"
df -h

printf "\nLinux Core:\n"
uname -r

printf "\nSystem OS:\n"
cat /etc/system-release

read -p "Do you want continue?[y/n]:" iscontinue
if [ "$iscontinue" == "n" ] || [ "$iscontinue" == "N" ]; then	
    printf "Just finished.\n"
    exit 1
fi

#configure selinx
printf "\nconfigure selinx... \n"

seStatus=`sestatus| awk '{print $3}'`
if [ "seStatus" != "disabled" ]; then
    sed -i 's/SELINUX=enforcing/SELINUX=disabled/g' /etc/sysconfig/selinux			#关闭selinux
    sed -i 's/SELINUXTYPE=targeted/#SELINUXTYPE=targeted/g' /etc/sysconfig/selinux
fi

printf "\nThe result of selinx configure file is :\n\n"
cat /etc/sysconfig/selinux

#configure iptables
chkconfig ip6tables off
printf "\ncheck iptables status :\n"				#配置iptables防火墙
service iptables status

read -p "Do you want to configure iptables?[y/n]:" isiptables
if [ "$isiptables" == "y" ] || [ "$isiptables" == "Y" ]; then
	read -p "Do you want to open port 3036 for mysql?[y/n]:" isopen3036			#打开3036端口用于mysql
	if [ "$isopen3036" == "y" ] || [ "$isopen3036" == "Y" ]; then
		hasSet=`grep "tcp \-\-dport 3036 \-j ACCEPT" /etc/sysconfig/iptables | wc -l`
		if [ "$hasSet" != "1" ]; then
			sed -i '/\-A INPUT \-m state \-\-state NEW \-m tcp \-p tcp \-\-dport 22 \-j ACCEPT/ a\\-A INPUT \-m state \-\-state NEW \-m tcp \-p tcp \-\-dport 3306 \-j ACCEPT' /etc/sysconfig/iptables		
		fi
	fi

	read -p "Do you want to open port 80 for http?[y/n]:" isopen80				#打开80端口用于http
	if [ "$isopen80" == "y" ] || [ "$isopen80" == "Y" ]; then
		hasSet=`grep "tcp \-\-dport 80 \-j ACCEPT" /etc/sysconfig/iptables | wc -l`
		if [ "$hasSet" != "1" ]; then
			sed -i '/\-A INPUT \-m state \-\-state NEW \-m tcp \-p tcp \-\-dport 22 \-j ACCEPT/ a\\-A INPUT \-m state \-\-state NEW \-m tcp \-p tcp \-\-dport 80 \-j ACCEPT' /etc/sysconfig/iptables
		fi
	fi

	read -p "Do you want to open port 443 for https?[y/n]:" isopen443			#打开443端口用于https
	if [ "$isopen443" == "y" ] || [ "$isopen443" == "Y" ]; then
		hasSet=`grep "tcp \-\-dport 443 \-j ACCEPT" /etc/sysconfig/iptables | wc -l`
		if [ "$hasSet" != "1" ]; then
			sed -i '/\-A INPUT \-m state \-\-state NEW \-m tcp \-p tcp \-\-dport 22 \-j ACCEPT/ a\\-A INPUT \-m state \-\-state NEW \-m tcp \-p tcp \-\-dport 443 \-j ACCEPT' /etc/sysconfig/iptables
		fi
	fi

	read -p "Do you want to open port 873 for rsync?[y/n]:" isopen873			#打开873端口用于rsync
	if [ "$isopen873" == "y" ] || [ "$isopen873" == "Y" ]; then
		hasSet=`grep "tcp \-\-dport 873 \-j ACCEPT" /etc/sysconfig/iptables | wc -l`
		if [ "$hasSet" != "1" ]; then
			sed -i '/\-A INPUT \-m state \-\-state NEW \-m tcp \-p tcp \-\-dport 22 \-j ACCEPT/ a\\-A INPUT \-m state \-\-state NEW \-m tcp \-p tcp \-\-dport 873 \-j ACCEPT' /etc/sysconfig/iptables
		fi
	fi

	cat /etc/sysconfig/iptables

	read -p "Do you want to restart iptables?[y/n]:" isrestart					#重启防火墙
	if [ "$isrestart" == "y" ] || [ "$isrestart" == "Y" ]; then
		service iptables restart
	fi
fi

#configure network
printf "\ncheck network params :\n"								#配置network
cat /etc/sysconfig/network

printf "\ncheck servername params :\n"
cat /etc/resolv.conf

printf "\ncheck ipconfig params :\n"
ifconfig

#configure time zone
printf "\nconfigure time zone :\n"
hasTimeZoneSet=`date -R | grep "+0800" | wc -l`
if [ "$hasTimeZoneSet" != "1" ]; then
	cp -f /usr/share/zoneinfo/Asia/Shanghai /etc/localtime
fi

#sync time
yum -y remove openntpd											#关闭openntpd，这个进程是个用于同步ntp服务器时间的守护进程

isntpd=`whereis ntpdate|awk '{print $2}'`
if [ "$isntpd" == "" ]; then									#如果ntp为空，则安装ntpdate服务器
    yum -y install ntpdate
fi

/usr/sbin/ntpdate us.pool.ntp.org								#同步时间
/usr/sbin/hwclock -w											#将系统时钟写入硬件时钟

is_ntpd_exists=`grep "/usr/sbin/ntpdate" /etc/rc.local | wc -l`	#如果ntpdate命令不是开机自启动
if [ "$is_ntpd_exists" == "0" ]; then
    echo "/usr/sbin/ntpdate us.pool.ntp.org >/dev/null">>/etc/rc.local	#将更新时间写进开机自启动脚本中
fi

is_crontab_exists=`grep "/usr/sbin/ntpdate" /etc/crontab | wc -l`		#写进定时脚本
if [ "$is_crontab_exists" == "0" ]; then
    echo "1	*	*	*	*	root	/usr/sbin/ntpdate us.pool.ntp.org >/dev/null">>/etc/crontab
fi

printf "\ncat /etc/rc.local :\n"
cat /etc/rc.local

printf "\ncat /etc/crontab :\n"
cat /etc/crontab 

postfixExisTs=`chkconfig --list | grep postfix | grep 5:on | wc -l`			#postfix是个邮件相关的代理
if [ "$postfixExisTs" == "1" ]; then
	read -p "Do you want to close postfix?[y/n]:" postfixUsed
	if [ "$postfixUsed" == "y" ] || [ "$postfixUsed" == "Y" ]; then
		chkconfig postfix off											#关闭开机自启动

	fi	
fi
sendmailExisTs=`chkconfig --list | grep sendmail | grep 5:on | wc -l`		#sendmail也是个邮件相关的代理
if [ "$sendmailExisTs" == "1" ]; then
	read -p "Do you want to close sendmail?[y/n]:" postfixUsed
	if [ "$postfixUsed" == "y" ] || [ "$postfixUsed" == "Y" ]; then
		chkconfig sendmail off
	fi	
fi

read -p "Do you want to close RAID service?[y/n]:" raidClosed			#关闭raid
if [ "$raidClosed" == "y" ] || [ "$raidClosed" == "Y" ]; then
	chkconfig blk-availability off
	chkconfig lvm2-monitor off
	chkconfig udev-post off
fi

read -p "Do you want to close nfs service?[y/n]:" nfsClosed
if [ "$nfsClosed" == "y" ] || [ "$nfsClosed" == "Y" ]; then
	chkconfig netfs off
fi

#set vim editor 1tab=4space
printf "\nconfigure vim editor 1tab=4space :\n"						#配置vim编辑器，1tab等于4个空格
if [ ! -f /root/.vimrc ]; then
cat >/root/.vimrc<<EOF
set tabstop=4
set softtabstop=4
set shiftwidth=4
set noexpandtab
set nu
set autoindent
set cindent
EOF
fi

#close Control-Alt-Deletepressed shutdown server
sed -i 's/^exec \/sbin\/shutdown/#exec \/sbin\/shutdown/g' /etc/init/control-alt-delete.conf	#停用按下Ctrl-Alt-Del 重启系统的功能。

printf "============== The End. ==============\n"