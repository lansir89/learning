window.onload=replace_place

function checkEmpty(blog_input_tmp,EditorTextarea_tmp)
{
    if(!EditorTextarea_tmp || !blog_input_tmp)
        return true;
    else
    {
        return false;
    }
}

function ajax_post(){
    post_xhr=new XMLHttpRequest();

    post_xhr.onreadystatechange=function()
    {
        if (post_xhr.readyState==4 && post_xhr.status==200)
        {
            document.getElementById("ajaxbox_id").innerHTML=post_xhr.responseText;
        }
  }
    post_xhr.open("get",arguments[0],true);
    post_xhr.setRequestHeader("Content-type","application/x-www-form-urlencoded");
    post_xhr.send(null);
    return false;
}

function setting_ajax(){
    post_xhr=new XMLHttpRequest();

    post_xhr.onreadystatechange=function()
    {
        if (post_xhr.readyState==4 && post_xhr.status==200)
        {
            document.getElementById("ajaxbox_id").innerHTML=post_xhr.responseText;
        }
  }
    post_xhr.open("GET",arguments[0],true);
    post_xhr.setRequestHeader("Content-type","application/x-www-form-urlencoded");
    post_xhr.send(null);
}

function blog_submit(){
    var content=document.getElementById("id_body").value;
    content=content.replace(/\n/g,"<br />");
    document.getElementById("id_body").value=content;
}

