from django.db import transaction
from django.http import HttpResponseRedirect as redirect
from django.db.models import F
from iclick.models import Link


def click_count(request, token):
    with transaction.atomic():                          #定义数据库的原子操作。在这个类里开启原子操作为关闭原子操作
        try:
            link = Link.objects.select_for_update().filter(token=token).first()       #查询结果集中的第一条。select_for_update应该是取出所有数据，然后filter过滤
        except Link.DoesNotExist:
            pass
        else:
            to_url = link.to_url
            if to_url:
                link.count = F('count') + 1                         #count字段值加1。使用F对象来访问字段
                link.save()
                return redirect(to_url)                         #重定向到指定url。上面的count就是保存着这个url的访问次数
        return redirect('/')                                    #否则，重定向到首页
