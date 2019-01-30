<?php

### проверка timeout
if($auth->CheckAuth()){
   if($auth->CheckLifeTime()==false){
  	$umode="exit";
  	$err="Превышен интервал 'простоя'";
   }
}
###


#### Авторизация #######
if($auth->CheckAuth()==false) {

    $menu_js="";

    $auth->LifeTime=TIMEOUT;

    $err="Неверный Логин или Пароль!";
    //Ищем его в таблице временного доступа
    $auth->auth["oper_group"]="";
    $sqlstr="SELECT * FROM ".$DB["auth_tmp_access_v"]." WHERE ip='".$_SERVER["REMOTE_ADDR"]."' LIMIT 1";
    $db->query($sqlstr);
    if($db->next_record()) {
      $auth->auth["fio"]       = $db["fio"];
      $auth->auth["uid"]       = $db["uid"];
      $auth->auth["login"]     = $db["login"];
      $auth->auth["oper_group"]= $db["type"];
      $auth->auth["time"]      = datedigit(date("d.m.Y H:i:s"));
      $err = "";
    }else{
      $err = "Вы не зарегестрированы";
    }

    
    

    
    if($err=="") {
    	//save_auth_log("OK # ".$auth->auth["login"]." # ".$auth->auth["oper_group"]." #  #  # ".$_SERVER["REMOTE_ADDR"]);
    	Log::get("auth", "OK", $auth->auth["oper_group"]);
     	$auth->LogIn();
      $auth->auth["pass"]="";
      $auth->auth["SSL"]="null";


    	$auth->auth["user_SSL"]="";
    	$auth->auth["grp_SSL"]="none";

    	if(!empty($_SERVER["SSL_CLIENT_CERT"]) && strlen($_SERVER["SSL_CLIENT_CERT"])>100) {
    	    $x = preg_replace("/\n$/","",$_SERVER["SSL_CLIENT_CERT"]);
    	    $x = "'".preg_replace("/\n/","&#&",$x)."'";
          $x = preg_replace("/\&#\&$/","",$x);
    	    $xx=`/www/apache2/cgi-bin/readssl.pl $x 2>/dev/null`;
    	    //exec("/www/apache2/cgi-bin/read_ssl2www ".$x, $xx, $return);
    	    $auth->auth["grp_SSL"]=preg_replace("/Modulus=/","",ereg_replace("\n","",$xx));
    	    //echo $auth->auth["grp_SSL"];
    	}else{
    	    $auth->auth["grp_SSL"]="null";
    	    //$auth->auth["grp_SSL"]="1";
    	}

    	$auth->auth["user_cgrp"]=0;
    	$auth->auth["user_cgrpname"]="";
    	$auth->auth["user_csrv"]=0;
    	$auth->auth["user_csrvname"]="";

    	$auth->auth["user_grp"]=array();
    	$auth->auth["user_grpname"]=array();
    	$auth->auth["user_srv"]=array();
    	$auth->auth["user_srvname"]=array();
    	$auth->auth["sec_ref"]=15;











    ### Чтение прав ####
    	$sqlstr="SELECT a.uid as uid, g.gid as gid, s.sid as s_id, g.g_name as g_name, s.s_name as s_name
      FROM ".$DB["auth_access"]." a, ".$DB["auth_groups"]." g, ".$DB["auth_service"]." s WHERE
      a.uid='".$auth->auth["uid"]."'
      AND a.gid=g.gid AND g.sid=s.sid
    	ORDER BY g_name ASC";
    	$db->query($sqlstr);
    	$access=array();
    	while($db->next_record()) {
        $access[$db["uid"]]["gid"]=$db["gid"];
        $access[$db["uid"]]["g_name"]=$db["g_name"];
        $access[$db["uid"]]["s_id"]=$db["s_id"];
        $access[$db["uid"]]["s_name"]=$db["s_name"];
    	}

      $i=0;
      foreach($access as $uid=>$value)
      {
    		$attr=array();
    		$sqlstr="SELECT v.v_str as value
        FROM ".$DB["auth_objects_atributs"]." a2v,".$DB["auth_atributs"]." a,".$DB["auth_values"]." v WHERE
    		a2v.aid=a.aid	AND a2v.vid=v.vid AND a.a_name='access'
        AND (
        (a2v.id_object='".$uid."' AND a2v.type_object='".$ObjType["auth"]."')
        OR
        (a2v.id_object='".$value["gid"]."' AND a2v.type_object='".$ObjType["groups"]."')
        OR
        (a2v.id_object='".$value["s_id"]."' AND a2v.type_object='".$ObjType["service"]."')
        ) AND (
          a.a_name='access'
        )";

    		$db->query($sqlstr);
    		$access="";
    	 	while($db->next_record()) {
          // переменные пользовательского SSL
          $access.=$db["value"]."##";
       	}

    		$qwerty=false; //флаг, что нет прав доступа юзеру
    		if(preg_match("/".$auth->auth["SSL"]."##/", $access) || $access=="1##") { // ищем в переменной строку сертификата или 1
    		    $qwerty=true;
    		}

    		if($qwerty==true){ // инче, переменные данной группы не разрешены
    		    $i++;
    		    $auth->auth["user_access"][$i]=$uid;
    		    $auth->auth["user_grp"][$i]=$value["gid"];
    		    $auth->auth["user_srv"][$i]=$value["s_id"];
    		    $auth->auth["user_grpname"][$i]=$value["g_name"];
    		    $auth->auth["user_srvname"][$i]=$value["s_name"];
    		}
      }





    	if($i==0 && count($auth->auth["user_grp"])==0){
    	    $err="Ошибка сертификата!";
    	} else {
          // выбираем первю группу по-умолчанию
    	    $auth->auth["user_cgrp"]=$auth->auth["user_grp"][1];
    	    $auth->auth["user_cgrpname"]=$auth->auth["user_grpname"][1];
    	    $auth->auth["user_csrv"]=$auth->auth["user_srv"][1];
    	    $auth->auth["user_csrvname"]=$auth->auth["user_srvname"][1];
    	}

    	//save_auth_log("GR # ".$auth->auth["login"]." # ".$auth->auth["user_cgrp"]." # ".$auth->auth["user_csrv"]." # ".$auth->auth["SSL"]);
    	Log::get("auth", "GR", $auth->auth["user_cgrp"], $auth->auth["user_csrv"], $auth->auth["SSL"]);
      #----- получение массива переменных доступных групп
      set_user_atributs();

    }

    if($err=="") {
      //Меняем пользователя БД
      if(isset($auth->auth["attr"]["db_user"])) {
        $db->User=$auth->auth["attr"]["db_user"];
        $db->Password=substr(md5($auth->auth["attr"]["db_user"]."xaker-fucker"), 0, 13);
        $db->reconnect();
        $db->add_arg_session("@user_id='".$auth->auth["uid"]."'");   //переменная сессии

        //$db->query("UPDATE ".$DB["auth_users"]." SET ip='".$_SERVER["REMOTE_ADDR"]."' WHERE uid=@user_id");
      }
      if(isset($auth->auth["attr"]["dbo_user"])) {
        $dbo->ContextUserID=$auth->auth["uid"];
        $dbo->User=$auth->auth["attr"]["dbo_user"];
        $dbo->Password=strrev(substr($auth->auth["attr"]["dbo_user"],3));
        $dbo->reconnect();
      }
      if(!isset($auth->auth["attr"]["db_user"]) || !isset($auth->auth["attr"]["dbo_user"])) {
        $err="Нет прав доступа к БД!";
      }
    }

    if($err!="") {
    	//save_auth_log("NO # ".$uname." # ".$response." # ".$pass1." # ".$auth->sys["challenge"]." # ".$_SERVER["REMOTE_ADDR"]." # ".$auth->auth["SSL"]);
    	Log::get("auth", "NO", $response, $pass1, $auth->sys["challenge"], $auth->auth["SSL"]);
      $auth->LogOut();
    }

}


####### Выход #########
elseif(isset($umode) && $umode=="exit")
{
    //save_auth_log("EX # ".$auth->auth["login"]." #  #  # ".$auth->sys["challenge"]." # ".$_SERVER["REMOTE_ADDR"]);
    Log::get("auth", "EX", $auth->sys["challenge"]);
    $auth->LogOut();
    $menu_js="";
    unset($db,$db1,$dbo,$dbo1);
}

?>
