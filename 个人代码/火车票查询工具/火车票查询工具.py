# coding: utf-8

"""命令行火车票查看器

Usage:
    tickets [-dgktz] <from> <to> <date>

Options:
    -h, --help 查看帮助
    -d         动车
    -g         高铁
    -k         快速
    -t         特快
    -z         直达

Examples:
    tickets 上海 北京 2016-10-10
    tickets -dg 成都 南京 2016-10-10
"""

import requests
import urllib
from docopt import docopt
from prettytable import PrettyTable
from colorama import init, Fore

from stations import stations


init()

class TrainsCollection:

    header = '车次 车站 时间 历时 一等 二等 软卧 硬卧 硬座 无座'.split()

    def __init__(self, available_trains, options):
        """查询到的火车班次集合

        :param available_trains: 一个列表, 包含可获得的火车班次, 每个
                                 火车班次是一个字典
        :param options: 查询的选项, 如高铁, 动车, etc...
        """
        self.available_trains = available_trains
        self.options = options

    def _get_duration(self, raw_train):									#计算历时时间
        duration = raw_train.get('lishi').replace(':', '小时') + '分'
        if duration.startswith('00'):
            return duration[4:]
        if duration.startswith('0'):
            return duration[1:]
        return duration

    @property
    def trains(self):												#将函数设置为属性
        for raw_train in self.available_trains:						#raw_train就是返回的json数据
            train_no = raw_train['station_train_code']				#该车次编号，如D293
            initial = train_no[0].lower()							#车次编号第一个字典代表车次类型，即是动车还是快车
            if not self.options or initial in self.options:
                train = [
                    train_no,        
                    '\n'.join([Fore.GREEN + raw_train['from_station_name'] + Fore.RESET,
                               Fore.RED + raw_train['to_station_name'] + Fore.RESET]),
                    '\n'.join([Fore.GREEN + raw_train['start_time'] + Fore.RESET,
                               Fore.RED + raw_train['arrive_time'] + Fore.RESET]),
                    self._get_duration(raw_train),
                    raw_train['zy_num'],
                    raw_train['ze_num'],
                    raw_train['rw_num'],
                    raw_train['yw_num'],
                    raw_train['yz_num'],
                    raw_train['wz_num'],
                ]
                yield train											#这个属性是一个列表

    def pretty_print(self):
        pt = PrettyTable()											#定义一个格式化对象，固定用法
        pt._set_field_names(self.header)							#设置行头
        for train in self.trains:
            pt.add_row(train)										#添加行
        print(pt)													#打印


def cli():
    """Command-line interface"""
    arguments = docopt(__doc__)										#docopt的固定用法，arguments是参数字典
    from_station = stations.get(arguments['<from>'])				#获取命令行from参数的内容
    to_station = stations.get(arguments['<to>'])					#获取命令行to参数的内容
    date = arguments['<date>']										#获取命令行date参数的内容
    url = ('https://kyfw.12306.cn/otn/lcxxcx/query?'				#这个是网站查询接口
           'purpose_codes=ADULT&queryDate={}&'
           'from_station={}&to_station={}').format(
                date, from_station, to_station
           )
    options = ''.join([												#将参数连接成一个新字符串
        key for key, value in arguments.items() if value is True	#key是指<from>、<to>等内容，value是指用户在命令行输入的参数
    ])
    r = requests.get(url, verify=False)								#获取网页json数据
    available_trains = r.json()['data']['datas']					#解释获取到的json内容
    TrainsCollection(available_trains, options).pretty_print()


if __name__ == '__main__':
    cli()