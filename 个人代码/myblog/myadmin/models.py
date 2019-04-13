# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models
# Create your models here.

class article(models.Model):
    title = models.CharField('标题', max_length=100)
    body = models.TextField('正文')

    created_time = models.DateTimeField('创建时间', auto_now_add=True)
    # auto_now_add : 创建时间戳，不会被覆盖

    last_modified_time = models.DateTimeField('修改时间', auto_now=True)
    # auto_now: 自动将当前时间覆盖之前时间

    abstract = models.CharField('摘要（可选）', max_length=200, blank=True, null=True,
                                help_text="可选项，若为空则摘取正文前200个字符")
    # 阅读量
    views = models.PositiveIntegerField('浏览量', default=0)
    # 点赞数
    likes = models.PositiveIntegerField('点赞数', default=0)
    # 是否置顶
    topped = models.BooleanField('置顶', default=False)
    # 目录分类
    # on_delete 当指向的表被删除时，将该项设为空
   # category = models.ForeignKey('Category', verbose_name='分类',
    #                             null=True,
    #                             on_delete=models.SET_NULL)
    # 标签云
   # tags = models.ManyToManyField('Tag', verbose_name='标签集合', blank=True)

    class Meta:
        # Meta 包含一系列选项，这里的ordering表示排序, - 表示逆序
        # 即当从数据库中取出文章时，以文章最后修改时间逆向排序
        ordering = ['-last_modified_time']

class comment(models.Model):
    user_com=models.CharField('用户名',max_length=50)
    body_com=models.TextField('评论内容')
    time_com=models.DateTimeField(auto_now_add=True)
    article_id=models.IntegerField(blank=False,default=0)


