#-*_coding:utf8-*-

"""命令行汽车票查看器

Usage:
    ticket <from> <to> <date> [ZHEN]

Options:
    -h, --help 查看帮助
    <from>      始发站
    <to>        终点站
    <date>      日期
    [zhen]      可选，到哪个镇

Examples:
    ticket 广州 罗定 2016-10-10
    ticket 广州 罗定 2016-10-10 泗纶
"""
import requests
import re
import sys
from urllib import quote
from docopt import docopt
from texttable import Texttable

def get(html):
    coachtime = re.compile(r'<td class="coach_time"><span class="coach_time_t">(.*?)</span></td>')
    coachtimelist = coachtime.findall(html)
    coachss = re.compile(r'<div class="st_ft_14"><span class="tb_coach_ss">发</span><span title=(.*?)>')
    coachsslist = coachss.findall(html)
    coachds = re.compile(r'div class="st_ft_14"><span class="tb_coach_ds">到<\/span><span title=(.*?)>')
    coachdslist = coachds.findall(html)
    coachnum = re.compile(r'<td class="coach_num">\s+<div>(.*?)<\/div>')
    coachnumlist = coachnum.findall(html)
    coachnum_2 = re.compile(r'</div>\s+<div>(.*?)<\/div>')
    coachnumlist_2 = coachnum_2.findall(html)
    price = re.compile(r'<span class="coach_price_oringe"><span class="st_ft_12">\￥<\/span>(.*?)\s+</span>')
    pricelist = price.findall(html)
    if len(coachdslist) == len(coachnumlist_2) == len(coachsslist) == len(coachnumlist) == len(pricelist):
        for i in xrange(len(coachsslist)):
            bus = []
            bus.append(coachtimelist[i])
            bus.append(coachsslist[i].strip('"'))
            bus.append(coachdslist[i].strip('"'))
            bus.append(coachnumlist[i])
            bus.append(coachnumlist_2[i].decode())
            bus.append(pricelist[i])
            table.add_row(bus)
            del bus

reload(sys)
sys.setdefaultencoding('utf-8')
headers =['发车时间','发车','到达站','车型','车次','票价']
table = Texttable()
table.set_deco(Texttable.HEADER)
table.set_cols_dtype(['a',  # text
                      'a',
                      'a',
                      'a',
                      'a',
                      'a'])
table.set_cols_align(["c", "c", "c", "c", "c","c"])
table.header(headers)
arguments = docopt(__doc__)
s=quote(arguments["<from>"])
d=quote(arguments["<to>"])
t=arguments["<date>"]
baseUrl='http://xq.zuoche.com/xqweb/coach.jspx?s=%s&d=%s&t=%s'%(s,d,t)
htmlpage=requests.get(baseUrl).content
findpage = re.compile(r"<a class='pagebar_pages'  href='javascript:goPage\(.*?\)'>(.*?)<\/a>&nbsp;")  # 找到总共有多少页
pageindexlist = findpage.findall(htmlpage)  # 这里result保存的是页列表
get(htmlpage)
for i in pageindexlist:
    url="%s&p=%d"%(baseUrl,int(i))
    htmlpage = requests.get(url).content
    get(htmlpage)
print table.draw()