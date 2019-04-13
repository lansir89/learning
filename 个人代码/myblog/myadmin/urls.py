from django.conf.urls import url
from . import views

urlpatterns = [
    url(r'^my-admin$',views.myadmin.as_view(),name='djMy-admin'),
    url(r'^admin$',views.mylogin.as_view(),name='djAdmin'),
    url(r'^manage$',views.manage.as_view(),name='djManage'),
    url(r'^manage/(?P<flag>\w{4,20})$',views.manage.as_view()),
    url(r'^writearticle$',views.writearticle.as_view(),name='djWritearticle'),
    url(r'^post_article$',views.post_article.as_view(),name='djPost_article'),
    url(r'^article$',views.list_article.as_view(),name='djArticle'),
    url(r'^setting$',views.djsetting.as_view(),name='djSetting'),
    url(r'^post_setting$',views.post_setting.as_view(),name='djPost_Setting'),
    url(r'^index$',views.articleView.as_view(),name='djIndex'),
    url(r'^news/(?P<pk>\d+)$',views.news.as_view(),name='djNews'),
    url(r'^news/(?P<pk>\d+)/comment$',views.commentCreate.as_view(),name='djComment'),
    url(r'^comment/(?P<pk>\d+)$',views.seccess.as_view(),name='djSeccess'),
]
