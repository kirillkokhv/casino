<?php
# последни изменения 04.05.2009
### проверка сохраненных сессий
if($auth->CheckAuth()==false && $cook["save_session"]==1 && !empty($cook["login"]) && !empty($_SERVER["REMOTE_ADDR"])) {
//if (1 == 1) {

    $dbo->query("
  	SELECT
  	    sessions,
  	    CASE WHEN actual_date <= CURRENT_DATE-11/24 THEN 1 ELSE 0 END need_update
	FROM auth_sessions
	WHERE ip='" . $_SERVER["REMOTE_ADDR"] . "'
	      AND login='" . $cook["login"] . "'
		  AND actual_date > CURRENT_DATE-1/2
  ");
    if ($dbo->next_record()) {
        $need_update     = $dbo["need_update"];
        $dbo["sessions"] = unserialize($dbo["sessions"]);
        $auth->auth      = $dbo["sessions"]["auth"];
        $menu_js         = $dbo["sessions"]["menu_js"];
        $sess->reg("menu_js");

        if (isset($auth->auth["attr"]["dbo_user"])) {
            $dbo->ContextUserID = $auth->auth["user_id"];
            if ($dbo->User != $auth->auth["attr"]["dbo_user"]) {
                $dbo->User     = $auth->auth["attr"]["dbo_user"];
                $dbo->Password = strrev(substr($auth->auth["attr"]["dbo_user"], 3));
                $dbo->reconnect();
            } else {
                $dbo->set_session();
            }
        }

        //if(isset($_SESSION["call_id"])) $_SESSION["call_id"]=-1;
        foreach ($_SESSION as $k => $v) if (strpos($k, "call_") === 0) unset($_SESSION[$k]);
        $auth->LifeTime = TIMEOUT;
        $auth->LogIn();
        if ($need_update == 1) {
            $dbo->query("
			UPDATE auth_sessions
			SET actual_date=CURRENT_DATE
			WHERE ip='" . $_SERVER["REMOTE_ADDR"] . "'
			      AND login='" . $cook["login"] . "'
		");
        }
        //Log::get("auth", "SAVE");
    }
}

### проверка timeout
if ($auth->CheckAuth()) {
    if ($auth->CheckLifeTime() == false) {
        $umode = "exit";
        //$err="Превышен интервал 'простоя'";
    }
}
###
set_vars(array("umode"), TYPE_STRING);
#### Авторизация #######
if ($umode == "log") {

    $auth->free_sess(); //удаляем данные от другого пользователя
    set_vars(array("uname", "response", "pass1"), TYPE_STRING);
    $menu_js = "";
    $auth->LifeTime = TIMEOUT;
    $err = "";
    $uname = STR::translit_kb($uname);
    $pass1 = STR::translit_kb($pass1);

    $cook["login"] = $uname;


        $sqlstr = "SELECT a.*
		FROM auth_users a WHERE login=LOWER('" . $uname . "')";
    /*
    if ($response == "") {
	   $sqlstr.=" AND u.passwd='".md5($pass1)."'";
    }
    */
    //$sqlstr.=" AND u.u_lock='0' AND u.u_deleted='0'";

    $dbo->query($sqlstr);

    if ($dbo->next_record()) {
        $auth->auth["uid"] = $dbo["user_id"];
        $auth->auth["user_id"] = $dbo["user_id"];
        $auth->auth["user_type"] = "manager";
        $auth->auth["lname"] = $dbo["lname"];
        $auth->auth["fname"] = $dbo["fname"];
        $auth->auth["mname"] = $dbo["mname"];
        $auth->auth["fio"] = $auth->auth["lname"] . " " . $auth->auth["fname"] . " " . $auth->auth["mname"];
        $auth->auth["login"] = $dbo["login"];
        $auth->auth["pass"] = $dbo["password"];
        $auth->auth["admin"] = $dbo["admin"];
        $auth->auth["time"] = datedigit(date("d.m.Y H:i:s"));
        $auth->auth["user_deleted"] = $dbo["deleted"];
        $auth->auth["user_id"] = $dbo["user_id"];
       
        $auth->auth["work_phone"] = $dbo["work_phone"];
        $auth->auth["mob_phone"] = $dbo["mob_phone"];
        $auth->auth["email"] = $dbo["email"];
        $auth->auth["avatar"] = $dbo["avatar"] != "" ? $dbo["avatar"] : "avatar.png";
        $auth->auth["view_mine_deals"] = $dbo["view_mine_deals"];
        $auth->auth["edit_mine_deals"] = $dbo["edit_mine_deals"];
        $auth->auth["view_mine_clients"] = $dbo["view_mine_clients"];
        $auth->auth["view_mine_stats"] = $dbo["view_mine_stats"];


    } else {
        $cook["login"] = "";

        $err = "Неверный Логин!";

        Log::get("auth", "NOLOGIN", $uname);

    }
    
   

    $IN_pass = md5($pass1); //если JS отключен, то строка = введенному юзером паролю
    if (!empty($auth->auth["pass"]) && !empty($auth->auth["login"])) {

        if ($response == "") {
            if ($IN_pass != $auth->auth["pass"]) {
                // JS отключен, пароль не подходит
                $err = "Неверный Пароль (JS)!";
            }
        } else {

            if (md5($auth->auth["login"] . ":" . $auth->auth["pass"] . ":" . $auth->sys["challenge"]) != $response) {
                // JS включен, пароль не подходит
                $err = "Неверный Пароль!";
            }
        }
    } else {
        $err = "Ошибка подключения к БД"; //Исключительный случай, ошибка при получении данных из БД
    }
    if ($err != "") {
        Log::get("auth", "NOPASS", $response, md5($auth->auth["login"] . ":" . $pass1 . ":" . $auth->sys["challenge"]));
    }

    //$auth->auth["pass"]="";
    /*if($err=="") {
      if($auth->auth["user_lock"]=="1") {
        $err="Пользователь Заблокирован";
        Log::get("auth", "LOCKED", $auth->auth["login"], $auth->auth["user_id"]);
      }
      if($auth->auth["user_deleted"]=="1") {
        $err="Пользователь Удален";
        Log::get("auth", "DEL", $auth->auth["login"], $auth->auth["user_id"]);
      }
         
    }*/

    $auth->auth["SSL"] = "null";

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    if ($err == "") {

        //save_auth_log("OK # ".$uname." # ".$response." # ".$pass1." # ".$auth->sys["challenge"]." # ".$_SERVER["REMOTE_ADDR"]);
        $auth->LogIn();
       /* if (!empty($_SERVER["SSL_CLIENT_CERT"]) && strlen($_SERVER["SSL_CLIENT_CERT"]) > 100) {

            $x  = preg_replace("/\n$/", "", $_SERVER["SSL_CLIENT_CERT"]);
            $x  = "'" . preg_replace("/\n/", "&#&", $x) . "'";
            $x  = preg_replace("/\&#\&$/", "", $x);
            $xx = `/www/apache2/cgi-bin/readssl.pl $x 2>/dev/null`;
            //exec("/www/apache2/cgi-bin/read_ssl2www ".$x, $xx, $return);
            $auth->auth["SSL"] = preg_replace("/Modulus=|\n/", "", $xx);

        } else {
            $auth->auth["SSL"] = "none";
        }
        Log::get("auth", "OK", $auth->auth["SSL"]);*/


        $auth->auth["user_groups"]        = array();
        $auth->auth["user_group_changed"] = "";


        ### Чтение прав ####
  
            $sqlstr = "
      SELECT
        g.group_id   group_id,
        g.name       group_name
      FROM auth_access a, auth_groups g
      WHERE a.user_id=" . $auth->auth["user_id"] . " AND a.group_id=g.group_id AND g.locked=0
    	";

            $dbo->query($sqlstr);

            $access = array();
            $i = 0;
            while ($dbo->next_record()) {
                $access[$i]["group_id"] = $dbo["group_id"];
                $access[$i]["group_name"] = $dbo["group_name"];
                $i++;
            }

            $i = "";
            foreach ($access as $value) {
                $attr = array();
                //проверям сертификат на группе
                $sqlstr = "
        SELECT v.text value
        FROM " . $DB["auth_objects_atributs"] . " a2v, " . $DB["auth_values"] . " v
        WHERE
      		a2v.atribut_id=1 AND a2v.value_id=v.value_id
          AND (
            (a2v.object_id=" . $auth->auth["user_id"] . " AND a2v.object_type=" . $ObjType["auth"] . ")
            OR
            (a2v.object_id=" . $value["group_id"] . " AND a2v.object_type=" . $ObjType["groups"] . ")
            )
        ORDER BY a2v.object_type DESC
        ";
                $dbo->query($sqlstr);
                $access_str = "";
                while ($dbo->next_record()) {
                    // переменные пользовательского SSL
                    $access_str .= $dbo["value"] . "##";
                }

                $qwerty = false; //флаг, что нет прав доступа юзеру
                if (preg_match("/" . $auth->auth["SSL"] . "##/", $access_str) || $access_str == "1##" || $_SERVER["REMOTE_ADDR"] == "127.0.0.1") { // ищем в переменной строку сертификата или 1
                    $qwerty = true;
                }
                $qwerty = true;
                if ($qwerty == true) { // инче, переменные данной группы разрешены
                    if ($i == "") $i = $value["group_id"];
                    $auth->auth["user_groups"][$value["group_id"]]["group_id"] = $value["group_id"];
                    $auth->auth["user_groups"][$value["group_id"]]["group_name"] = $value["group_name"];

                }
            }


            if ($i == "") {
                Log::get("auth", "NOSSL", $auth->auth["SSL"], $access_str);
                $err = "Ошибка сертификата!";
            } else {
                // выбираем первю группу по-умолчанию
                $auth->auth["user_group_changed"] = $i;


                #----- получение массива переменных доступных групп
                set_user_atributs();
            }
        

    }
/*
    if ($err == "") {
        //Меняем пользователя БД
        /*
        if(isset($auth->auth["attr"]["db_user"])) {
          $db->User=$auth->auth["attr"]["db_user"];
          $db->Password=substr(md5($auth->auth["attr"]["db_user"]."xaker-fucker"), 0, 13);
          $db->reconnect();
          $db->add_arg_session("@user_id='".$auth->auth["user_id"]."'");   //переменная сессии

          //$db->query("UPDATE ".$DB["auth_users"]." SET ip='".$_SERVER["REMOTE_ADDR"]."' WHERE uid=@user_id");
        }

        if (isset($auth->auth["attr"]["dbo_user"])) {
            $dbo->ContextUserID = $auth->auth["user_id"];
            if ($dbo->User != $auth->auth["attr"]["dbo_user"]) {
                $dbo->User = $auth->auth["attr"]["dbo_user"];

                $dbo->Password = strrev(substr($auth->auth["attr"]["dbo_user"], 3));
                $dbo->reconnect();
            } else {
                $dbo->set_session();
            }
        }

        /* if(!isset($auth->auth["attr"]["dbo_user"])) {
           $err="Нет прав доступа к БД!";
           Log::get("auth", "NOBD");
         }
    }
*/
    if ($err != "") {
        //save_auth_log("NO # ".$uname." # ".$response." # ".$pass1." # ".$auth->sys["challenge"]." # ".$_SERVER["REMOTE_ADDR"]." # ".$auth->auth["SSL"]);
        $auth->LogOut();
    }

    if ($err == "") {
        //сохраняем инфо о последнем визите
        $dbo->query("
	        INSERT INTO " . $DB["auth_exp"] . " (user_id,ip, last_date)  VALUES (" . $auth->auth["user_id"] . ", '" . $_SERVER["REMOTE_ADDR"] . "', CURRENT_DATE) on duplicate key
                    update ip='" . $_SERVER["REMOTE_ADDR"] . "', last_date=CURRENT_DATE
          
		");


    }

} ####### Выход #########
elseif ($umode == "exit") {
    //save_auth_log("EX # ".$auth->auth["login"]." #  #  # ".$auth->sys["challenge"]." # ".$_SERVER["REMOTE_ADDR"]);
    Log::get("auth", "EXIT");
    $dbo->query("DELETE FROM " . $DB["auth_sessions"] . " WHERE login='" . $auth->auth["login"] . "' AND ip='" . $_SERVER["REMOTE_ADDR"] . "'");
    $auth->LogOut();
    $cook["save_session"] = 0;
    $menu_js              = "";
    $reason_pass          = "";
    $auth->free_sess(); //удаляем данные сессии
}
?>