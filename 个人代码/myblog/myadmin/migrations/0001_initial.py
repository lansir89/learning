# -*- coding: utf-8 -*-
# Generated by Django 1.11.6 on 2018-05-06 12:23
from __future__ import unicode_literals

from django.db import migrations, models


class Migration(migrations.Migration):

    initial = True

    dependencies = [
    ]

    operations = [
        migrations.CreateModel(
            name='article',
            fields=[
                ('id', models.AutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('title', models.CharField(max_length=100, verbose_name='\u6807\u9898')),
                ('body', models.TextField(verbose_name='\u6b63\u6587')),
                ('created_time', models.DateTimeField(auto_now_add=True, verbose_name='\u521b\u5efa\u65f6\u95f4')),
                ('last_modified_time', models.DateTimeField(auto_now=True, verbose_name='\u4fee\u6539\u65f6\u95f4')),
                ('abstract', models.CharField(blank=True, help_text='\u53ef\u9009\u9879\uff0c\u82e5\u4e3a\u7a7a\u5219\u6458\u53d6\u6b63\u6587\u524d54\u4e2a\u5b57\u7b26', max_length=54, null=True, verbose_name='\u6458\u8981\uff08\u53ef\u9009\uff09')),
                ('views', models.PositiveIntegerField(default=0, verbose_name='\u6d4f\u89c8\u91cf')),
                ('likes', models.PositiveIntegerField(default=0, verbose_name='\u70b9\u8d5e\u6570')),
                ('topped', models.BooleanField(default=False, verbose_name='\u7f6e\u9876')),
            ],
            options={
                'ordering': ['-last_modified_time'],
            },
        ),
    ]
