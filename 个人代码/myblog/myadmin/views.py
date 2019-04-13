#coding=utf-8

from __future__ import unicode_literals
from django.shortcuts import render,HttpResponseRedirect
from django.http import HttpResponse
from django.contrib.auth import authenticate,login
from models import article,comment
from django.core.urlresolvers import reverse
from django.views.generic.base import View
from form import articleForm,comForm
from django.views.generic import ListView,DetailView
import re
import sys
default_encoding = 'utf-8'

# Create your views here.

#返回的是后台默认那个文章页面
class manage(View):
    def get(self,request,flag=""):
        content={}
        if not request.user.is_authenticated():
            return HttpResponseRedirect('my-admin.html')
        if(flag=="setting"):
            return render(request, 'admin.html',{'setting_result':'True'})      #网站设置提交成功，则显示设置成功
        if (flag=="article"):
            return render(request, 'admin.html', {'article': 'True'})           #文章提交成功
        content["article_count"]=article.objects.count()
        content['com_count']=comment.objects.count()
        content['article_list']=article.objects.all()[:17]
        content['mycom']=comment.objects.all()
        return render(request,'admin.html',content)

#登录视图
class myadmin(View):
    def get(self,request):
        return render(request,'my-admin.html')

#登录判断
class mylogin(View):
    def post(self,request):
        username=request.POST['username']
        password=request.POST['password']
        request.session['ssusername'] = username
        request.session['sspwd'] = password
        user=authenticate(username=username,password=password)
        if user is not None:
            if user.is_active:
                login(request,user)
                return HttpResponseRedirect(reverse('djManage'))
        return HttpResponse("登陆失败")

#编写文章视图
class writearticle(View):
    def get(self,request):
        if not request.user.is_authenticated():
            return HttpResponseRedirect('my-admin')
        form=articleForm()
        content={"form":form}
        return render(request,'writearticle.html',content)

#提交文章视图
class post_article(View):
    def post(self,request):
        form = articleForm(request.POST)
        if form.is_valid():                         # 验证数据是否合法
            url = "".join([reverse('djManage'), "/article"])
            newTitle=form.cleaned_data['title']
            newBody=form.cleaned_data['body']
            if form.cleaned_data['abstract'] is None:
                newAbstract=newBody[:200]
            else:
                newAbstract = form.cleaned_data['abstract']
            newAbstract.replace('\s','')
            newArticle=article(title=newTitle,body=newBody,abstract=newAbstract)
            print newArticle.body
            newArticle.save()
            return HttpResponseRedirect(url)

#后台默认页面
class admin_index(View):
    def get(self,requeat):
        myarticle=article.objects.count()
        return HttpResponse(myarticle)

class list_article(View):
    def get(self,request):
        return HttpResponse("文章页面")

#设置页面视图
class djsetting(View):
    def get(self,request):
        if sys.getdefaultencoding() != default_encoding:
            reload(sys)
            sys.setdefaultencoding(default_encoding)
        with open('templates/base.html','r+') as f:
            set_con=f.read()
            djtitle=re.search(r'<title>(.*?)</title>',set_con).group(1)
            djkeywords=re.search(r'"keywords" content="(.*?)">',set_con).group(1)
            djdescription = re.search(r'"description" content="(.*?)">', set_con).group(1)
            with open(r'templates/setting.html','r+') as setfile:
                setfilecon=setfile.read()
                setfilecon=replace_con(r'id="blog_title_id" value="(.*?)">',setfilecon,djtitle,26,2)
                setfilecon=replace_con(r'id="blog_keywords_id" value="(.*?)">',setfilecon,djkeywords,29,2)
                setfilecon=replace_con(r'id="blog_description_id" value="(.*?)">',setfilecon,djdescription,32,2)
                f.close()
                setfile.close()
            with open(r'templates/setting.html','w') as setfile:
                setfile.truncate();
                setfile.write(setfilecon)
                setfile.close()
                return render(request, 'setting.html')

#通过文件字符串替换的形式来实现保存保存设置，设置是直接保存到模板文件里的
#这种实现方式问题太多，实现比较复杂
class post_setting(View):
    def post(self,request):
        title_tmp=request.POST["name_title"]
        keywords_tmp=request.POST["name_keywords"]
        description_tmp=request.POST["name_description"]
        with open('templates/base.html','r+') as file_tmp:
            file_con=file_tmp.read()
        with open('templates/base.html', 'w') as file_tmp:
            setfilecon = replace_con(r'<title>(.*?)</title>', file_con, title_tmp, 7, 8)
            setfilecon = replace_con(r'"keywords" content="(.*?)">', setfilecon, keywords_tmp, 20, 2)
            setfilecon = replace_con(r'"description" content="(.*?)">', setfilecon, description_tmp, 23, 2)
            file_tmp.truncate()
            if(setfilecon!=''):
                file_tmp.write(setfilecon)
            file_tmp.close()
            url="".join([reverse('djManage'),"/setting"])
            return HttpResponseRedirect(url)


#使用字符串连接的方式实现字符串替换，参数分别为要查找的正则表达式、文件内容、新的字符串以及两个偏移位置
def replace_con(rgExp,filecon,newcon,begin,end):
    try:
        tmp = re.search(rgExp, filecon)
        setfilecon = "".join([filecon[:tmp.start() + begin],newcon,filecon[tmp.end() - end:]])
    except:
        return False
    return setfilecon

class index(View):
    def get(self,request):
        return render(request, 'index.html',{'index_flag':True})

class articleView(ListView):
    model = article
    context_object_name = 'article_list'
    template_name = 'index.html'
    paginate_by = 10

class news(DetailView):
    model = article
    template_name = 'news.html'
    context_object_name = "article"
    pk_url_kwarg = 'pk'

    def get(self, request, *args, **kwargs):
        self.object = self.get_object()
        context = self.get_context_data(object=self.object)
        context['myform']=comForm()
        pk=self.kwargs.get(self.pk_url_kwarg)
        comment_list=comment.objects.filter(article_id=pk)
        mycomment=comment_list
        context['comm']=mycomment
        print comment.objects.count()
        print mycomment
        return self.render_to_response(context)

class commentCreate(View):
    def post(self,request,pk):
        newCom=comForm(request.POST)
        if newCom.is_valid():
            newUser = newCom.cleaned_data["user_com"]
            newBody = newCom.cleaned_data["body_com"]
            newArticle_id = pk
            newcomment=comment(user_com=newUser,body_com=newBody,article_id=newArticle_id)
            newcomment.save()
            print '评论总数',comment.objects.count()
            return HttpResponseRedirect(reverse("djSeccess",args=[pk]))

class seccess(View):
    def get(self,request,pk=''):
        if pk is not None:
            url_pk=reverse("djNews",args=[pk])
            return render(request, 'seccess.html',{'url_pk':url_pk})

class com_list(ListView):
    model = comment
    context_object_name = 'article_list'
    template_name = 'index.html'
    paginate_by = 10