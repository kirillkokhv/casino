<?php
#### Авторизация #######
$auth->LogOut();
$auth->LifeTime=0; // бесконечная сессия
$auth->auth["uid"]=$auth->auth["user_id"]=0;
$auth->auth["fio"]=$auth->auth["lname"]="нет авторизации";
$auth->auth["login"]="without_auth";


$auth->auth["user_groups"]=array();
$auth->auth["user_group_changed"]=0;
$auth->auth["user_groups"][0]["group_id"]     = 0;
$auth->auth["user_groups"][0]["group_name"]   = "";
$auth->auth["user_groups"][0]["service_id"]   = 0;
$auth->auth["user_groups"][0]["service_name"] = "";

$auth->auth["admin"]=0;
$auth->auth["time"]=datedigit(date("d.m.Y H:i:s"));
$menu_js="";


//save_auth_log("OK # ".$auth->auth["login"]." # sis #  #  # ".$_SERVER["REMOTE_ADDR"]);
$auth->LogIn();
/*
$db->User="without_auth";
$db->Password=substr(md5($db->User."xaker-fucker"), 0, 13);
$db->reconnect();
*/

$dbo->User="root";
$dbo->Password="";
$dbo->reconnect();
?>
