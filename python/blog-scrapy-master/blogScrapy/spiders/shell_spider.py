#!/usr/bin/python2.7
#coding=utf-8

from scrapy.spider import BaseSpider
from scrapy.selector import HtmlXPathSelector
from scrapy.http import Request
from blogScrapy.items import CateItem,TagItem,BlogItem
import re,sys

#不同的BaseSpider子类爬取不同的内容，这些不同的内容通过选择器来进行选择
class ShellCateSpider(BaseSpider):
    """
    用于抓取shell网上的分类信息，并存储或到处为json文件
    """
    name = "shellcate"								#爬虫名
    allowed_domains = ['shell909090.com']			#允许爬取的域名
    start_urls = [									#从这个域名开始
        'http://shell909090.com/blog/',
        ]
    
    def parse(self, response):						#回调函数
        hxs = HtmlXPathSelector(response)		#定义xpath选择器
        
        items = []
        
        lis = hxs.select('//li')				#选择所有的li标签
        for li in lis:
            cl = li.select('@class').extract()	#选取属性class，并提取网页内容

            if len(cl) > 0 and re.match(r'^cat-item\ cat-item-\d{2,3}$', cl[0]):
                cate = li.select('a/text()').extract()[0]
                item = CateItem()				#这是一个item对象，items元组保存item对象。
                item['cate'] = cate
                items.append(item)
        
        return items

class ShellTagSpider(BaseSpider):
    """
    用于抓取shell网上的tag信息，并存储或导出为json文件
    """
    name = "shelltag"
    allowed_domains = ['shell909090.com']
    start_urls = [
        'http://shell909090.com/blog',
        ]
    
    def parse(self, response):
        hxs = HtmlXPathSelector(response)
        
        items = []
        
        tags = hxs.select('//div[@class="tagcloud"]/a/text()').extract()
        print tags
        items = []			
        for tag in tags:
            item = TagItem()
            item['tag'] = tag
            items.append(item)
        
        return items

class ShellBlogSpider(BaseSpider):
    """
    用于抓取blog内容，根据sitemap内容来找到博客链接
    """
    name = "shellblog"
    allowed_domains = ['shell909090.com']
    start_urls = [
        'http://shell909090.com/blog/sitemap.xml'
        ]
    
    def parse(self, response):				#parse本身就是回调函数
         hxs = HtmlXPathSelector(response)
         urls = hxs.select('//url/loc/text()').extract()		#根据选择器提取内容
        
         items = []
        
         debug = True			#如果是调试，则获取一个链接，否则，全部获取
         if debug:
             url = urls[1]

			#make_requests_from_url生成了一个request对象，replace函数将回调函数替换为指定的函数
             items.append(self.make_requests_from_url(url).replace(callback=self.parse_blog))	#生成一个request，并将回调函数替换为parse_blog
             return items
         for url in urls[1:]:
             items.append(self.make_requests_from_url(url).replace(callback=self.parse_blog))
         return items
    
    def parse_blog(self, response):
        hxs = HtmlXPathSelector(response)

        title = hxs.select('//h1[@class="entry-title"]/text()').extract() or ['']		#提取数据
        time = hxs.select('//time[@class="entry-date"]/text()').extract() or ['']
        blog = hxs.select('//div[@class="entry-content"]').extract() or ['']
        author = hxs.select('//footer/a[1]/text()').extract() or ['']
        cate = hxs.select('//footer/a[2]/text()').extract() or ['']
        tag = hxs.select('//footer/a[3]/text()').extract() or ['']
        comments = hxs.select('//ol[@class="commentlist"]/li').extract() or ['']

        item = BlogItem()
        item['title'] = title[0]
        item['pub_date'] = time[0]
        item['blog'] = blog[0]
        item['cate'] = cate[0]
        item['comments'] = comments
        item['tag'] = tag
        item['author'] = author
        return item
