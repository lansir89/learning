#coding=utf-8

from .models import article,comment
from  django import forms

class articleForm(forms.ModelForm):
    class Meta:
        model=article
        fields =['title','body','abstract']
        widgets = {
            'title': forms.TextInput(attrs={'class': 'blog_input','placeholder':"请输入标题"}),
            'body': forms.Textarea(attrs={'placeholder':"请输入正文",'class':"textarea"}),
            'abstract': forms.Textarea(attrs={'placeholder':"请输入摘要，限200个字",'class':"abstract"}),
        }

class comForm(forms.ModelForm):
    class Meta:
        model=comment
        fields=['user_com','body_com']
        widgets={
            'user_com':forms.TextInput(attrs={'class': 'com_user','placeholder':"请输入标题"}),
            'body_com':forms.Textarea(attrs={'class':"my_com_body",'placeholder':"请输入评论内容"}),
        }