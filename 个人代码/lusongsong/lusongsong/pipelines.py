# -*- coding: utf-8 -*-

# Define your item pipelines here
#
# Don't forget to add your pipeline to the ITEM_PIPELINES setting
# See: http://doc.scrapy.org/en/latest/topics/item-pipeline.html
from scrapy.exceptions import DropItem
from lusongsong.items import LusongsongTextItem
import sys

class LusongsongPipeline(object):
    def process_item(self, item, spider):
        reload(sys)                         
        sys.setdefaultencoding('utf-8') 
        if isinstance(item, LusongsongTextItem):
            filename = item['title'].strip() + '.txt'		
            f=open(filename,'w')
            f.write(str(item['blog_content']))
            raise DropItem('save finish')	
        return item
