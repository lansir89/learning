function checkpwd(){
    if(!document.getElementById){
            return false;
        }
    pwd=document.getElementById("input_js");
    username=document.getElementById("user_result");
    if(checkEmpty(username.value,pwd.value)){
        if(!pwd.value)
        {
            pwd.focus();
        }
        if(!username.value)
        {
            username.focus();
        }
        if(document.getElementById("pwd_result").style.display=="")
        {
            document.getElementById("pwd_result").style.display="inline";
            
        }
        return false;
    }
}

function checkEmpty(usertmp,pwdtmp)
{
    if(!usertmp || !pwdtmp)
        return true;
    else
    {
        return false;
    }
}