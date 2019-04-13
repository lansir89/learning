#!/usr/bin/python2.7
#coding=utf-8

# Define your item pipelines here
#
# Don't forget to add your pipeline to the ITEM_PIPELINES setting
# See: http://doc.scrapy.org/topics/item-pipeline.html

import sqlite3
from scrapy.exceptions import DropItem
from blogScrapy.items import CateItem,TagItem,BlogItem
from blogScrapy.helps import BlogSaveHelp
import sys,traceback,time


class BlogscrapyPipeline(object):
    def process_item(self, item, spider):
        return item

class BlogSavePipeline(object):
    """
    用于将blog的相关信息保存在sqlite表中
    """
    
    def __init__(self):
        self.help = BlogSaveHelp.BlogSaveHelps()		#help是BlogSaveHelps对象

    def process_item(self, item, spider):				#在这里保存数据
        if isinstance(item, CateItem):					#如果item是CateItem类型的，就保存item。CateItem是一个数据类
            self.help.saveCate(item['cate'])
            raise DropItem('cate save finish')			#发送DropItem异常。该异常由item pipeline抛出，用于停止处理item
        
        elif isinstance(item, TagItem):
            self.help.saveTag(item['tag'])
            raise DropItem('tag save finish')			#此外还有CloseSpider、IgnoreRequest、NotConfigured、NotSupported等异常
        
        # blog save,include comment, category and tags
        elif isinstance(item, BlogItem):
            self.saveBlog(item)  
            raise DropItem('blog save finish')
        return item
