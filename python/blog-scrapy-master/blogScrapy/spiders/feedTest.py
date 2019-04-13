from scrapy.contrib.spiders import XMLFeedSpider
from blogScrapy.items import BlogscrapyItem

class FeedtestSpider(XMLFeedSpider):			#爬虫类
    name = 'feedTest'							#这个应该是爬虫的名字
    allowed_domains = ['shell909090.com']		#允许的域名列表
    start_urls = ['http://www.shell909090.com/feed.xml']		#要爬取的域名
    iterator = 'iternodes' # you can change this; see the docs	#使用iternodes迭代器，iternodes是一个高性能的基于正则表达式的迭代器
    itertag = 'item' # change it accordingly

    def parse_node(self, response, selector):			#当节点符合提供的标签名时(itertag)该方法被调用
        i = BlogscrapyItem()
        #i['url'] = selector.select('url').extract()
        #i['name'] = selector.select('name').extract()
        #i['description'] = selector.select('description').extract()
        return i
