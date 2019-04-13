# -*- coding: utf-8 -*-

# Define here the models for your scraped items
#
# See documentation in:
# http://doc.scrapy.org/en/latest/topics/items.html

from scrapy.item import Item, Field


class LusongsongItem(Item):
    # define the fields for your item here like:
    # name = scrapy.Field()
    pass

class LusongsongBaseItem(Item):
    """
    只保留博客url地址和网页内容
    """
    url = Field()
    html_content = Field()

class LusongsongTextItem(Item):
    """
    到处为json格式文件的item
    """
    title = Field()
    blog_content = Field()
    dian = Field()
