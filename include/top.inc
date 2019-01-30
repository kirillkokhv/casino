<?php
### TOP
if($auth->CheckAuth()){
	### СМЕНА ГРУППЫ ###
	set_vars(array("change_group"),TYPE_STRING);
    if(!empty($change_group)) {
		if( isset($auth->auth["user_groups"][$change_group]) ) {
    	    Log::get("auth", "GROUP", $auth->auth["user_groups"][$change_group]["group_id"], $auth->auth["user_groups"][$change_group]["service_id"], $auth->auth["SSL"]);
    	    $auth->auth["user_group_changed"]=$change_group;
		}

		#----- получение массива доступных групп
		set_user_atributs();
		unset($menu_js, $_SESSION["menu_js"]);
		$auth->auth["access_menu"]=array();

		//Меняем пользователя БД
		if(isset($auth->auth["attr"]["dbo_user"])) {
			$dbo->ContextUserID=$auth->auth["user_id"];
			$dbo->User=$auth->auth["attr"]["dbo_user"];
			$dbo->Password=strrev(substr($auth->auth["attr"]["dbo_user"],3));
			$dbo->reconnect();
		}
    }

    $GROUP_SELECT='';
	if(!empty($auth->auth["user_groups"])) {
	    if( count($auth->auth["user_groups"]) > 1 ){
			foreach($auth->auth["user_groups"] as $k=>$v) {
				$GROUP_SELECT.='<option value="'.$k.'"';
				if($auth->auth["user_group_changed"]==$k) $GROUP_SELECT.=' selected';
				$GROUP_SELECT.='>'.$v["group_name"].'</option>';
	    	}
	    	$GROUP_SELECT='<form action="'.MAIN_URL.'" method="post"><select name="change_group" class="sz0">'.$GROUP_SELECT.'</select><input type="image" src="'.IMG_URL.'arrow.png" border="0" align="absmiddle" alt="Выбор группы доступа" class="null"></form>';
	    } else {
			$GROUP_SELECT = $auth->auth["user_groups"][$auth->auth["user_group_changed"]]["group_name"];
	    }
	}

	//$p->set_block("head", "header");
	//$p->set_block("head","auth");
	if(isset($_REQUEST["COMPACT"])) $_SESSION["COMPACT"] = 1;
	$p->set_var(array(
		"TIMEOUT"         => $auth->LifeTime*60,

	));
} else {
	//$p->set_block("head","noauth");
	$p->set_var("USER_NAME", isset($auth->auth["fio"]) ? $auth->auth["fio"] : "");
}
?>