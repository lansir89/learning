# -*- coding: utf-8 -*-

from scrapy.spider import BaseSpider
from scrapy.selector import Selector
from scrapy.http import Request
from lusongsong.items import LusongsongTextItem
from time import sleep
import re,sys

class lusongsongSpider(BaseSpider):
    """
    用于抓取blog内容
    """
    name = "lusongsong"
    allowed_domains = ['lusongsong.com']
    start_urls = [
        'http://lusongsong.com/blog/',
        'http://lusongsong.com/info/',
        'http://lusongsong.com/yulu/',
        ]
    
    def parse(self, response):				#parse本身就是回调函数
         urls = response.selector.xpath('//div[@class="post"]/h2/a/@href').extract()		#根据选择器提取内容
         items = []
        
         debug = True			#如果是调试，则获取一个链接，否则，全部获取
         if debug:
             url = urls[0]
			#make_requests_from_url生成了一个request对象，replace函数将回调函数替换为指定的函数
             items.append(self.make_requests_from_url(url).replace(callback=self.parse_blog))	#生成一个request，并将回调函数替换为parse_blog
             return items
         for url in urls:
             sleep(1)
             yield self.make_requests_from_url(url).replace(callback=self.parse_blog)
    
    def parse_blog(self, response):
        title = response.selector.xpath('//div[@class="post-title"]/h1/a/text()').extract() or ['123']		#提取标题
        blog = response.selector.xpath('//dl[@id="post-1445"]').extract() or ['456']
        dian = response.selector.xpath('//span[@class="commentViewNums"]/text()').extract() or ['456']

        item = LusongsongTextItem()
        item['title'] = title[0]
        item['blog_content'] = blog[0]
        item['dian'] = dian[0]

        yield item
 
        urls = response.selector.xpath('//div[@id="container"]/div[2]/div/div[3]/ul/li[1]/a/@href').extract()
        for url in urls:
             yield self.make_requests_from_url(url).replace(callback=self.parse_blog)
