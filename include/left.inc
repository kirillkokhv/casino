<?php
### LEFT ###
if($auth->CheckAuth()&&$auth->auth["user_type"]!="client"){
# если юзер авторизирован
    ### Меню
    $client_num=0;
   
      $sqlstr="
      SELECT
          m.menu_id,
          m.name,
          m.url,
          m.counter,
          m.menu_group_id,
          ifnull(m.img,'settings') img,
          (select menu_group_name from auth_menu_group mg where mg.menu_group_id = m.menu_group_id) group_name
      FROM
          auth_menu m
      WHERE
        m.deleted=0 AND
        m.menu_group_id > 0 AND
        (
          EXISTS (
            SELECT null FROM ".$DB["auth_menu2objects"]." a WHERE
            a.menu_id=m.menu_id AND (
              (
              (a.object_id=".$auth->auth["user_id"]." AND a.object_type=".$ObjType["auth"].") or
              (a.object_id=".$auth->auth["user_group_changed"]." AND a.object_type=2)
                  
              
              )
            
            )
          )
         or ".$auth->auth["admin"]." = 1
        )
        order by m.menu_group_id, m.ord
      ";

      $dbo->query($sqlstr);
       $auth->auth["access_menu"] = "";
      while($dbo->next_record()) {
          if(!empty($dbo["url"])||$dbo["menu_id"]==1) {
             $name=$dbo["name"];
            
             //$auth->auth["access_menu"][$dbo["menu_group_id"]]["group_name"]=$dbo["group_name"];
	     
             $auth->auth["access_menu"][$dbo["menu_group_id"]][$dbo["menu_id"]]["url"] = $dbo["url"];
             $auth->auth["access_menu"][$dbo["menu_group_id"]][$dbo["menu_id"]]["title"] = $name;
             $auth->auth["access_menu"][$dbo["menu_group_id"]][$dbo["menu_id"]]["img"] = $dbo["img"];
	     
             $auth->auth["access"][]=$dbo["url"];
             
             $url=MAIN_URL.$dbo["url"];
             $menu = $auth->auth["access_menu"];
             
          } else {$url="";}
       
      }

      $menu_tree = "<li class=\"header\">МЕНЮ</li>";
      $admin_tree = "<li class=\"header\">АДМИНИСТРИРОВАНИЕ</li>";
      $stat_selected=$deep1=="stat"?"class=selected":"";
      $stat_tree = " <li class=\"header\">СТАТИСТИКА</li>";
      
      
      foreach($menu as $bid=>$m){
        
        foreach($m as $mid=>$mv){
          if($mv["url"]==$deep1."/".$deep2."/") $selected = "active"; 
          else $selected="";
          if($bid==1) $menu_tree.= "<li class=\"".$selected." \">
            <a href=".MAIN_URL.$mv["url"].">
            <i class=".$mv["ico"]."></i>
            <span>".$mv["title"]."</span>
          </a>";
          
          if($bid==2) $stat_tree.= "<li class=\"".$selected." \">
            <a href=".MAIN_URL.$mv["url"].">
            <i class=".$mv["ico"]."></i>
            <span>".$mv["title"]."</span>
          </a>";
			    if($bid==3) $admin_tree.="<li class=\"".$selected." \">
            <a href=".MAIN_URL.$mv["url"].">
            <i class=".$mv["ico"]."></i>
            <span>".$mv["title"]."</span>
          </a>";
        }
      }
      
      //$menu_js=forest2str(forest($tmp));
      //$menu_js=TreeMenu0,(isset($MenuParam))?$MenuParam:"",$auth->auth["uid"],$auth->auth["user_cgrp"],$auth->auth["user_csrv"],(isset($auth->auth["admin"]))?$auth->auth["admin"]:0);
      //$stat_tree.="</ul></li>";
      $_SESSION["menu_js"]=$menu_tree.$admin_tree.$stat_tree;
  
    
//$menu_tree.=$stat_tree;
    //Путь раздела
	$MENU_LINE = "";
    

    ### end Меню
    //$p->set_block("head","left_menu");

    
    $p->set_var(array(
		"MENU_TREE"        => $admin_tree,
//	"ADMIN_TREE"        => $admin_tree,
 //   "STAT_TREE"         => $stat_tree,	
		"TOURIST_COUNT"	    => $client_num,
		"MENU_ID_CUR"       => isset($auth->auth["access_menu"][URI]["id"]) ? $auth->auth["access_menu"][URI]["id"] : 0,
		"POLL_IN_HIDDEN"    => "true",
		"POLL_IN_COLLAPSED" => "true",
		"LEFT_MENU_WIDTH"   => 215,


    ));

} else {

    # если юзер НЕ авторизирован
    
    $server = explode(".",$_SERVER["SERVER_NAME"]);
    if(count($server)>1) $db_name= $server[0];

    ##### Форма логина #####
    if(!isset($err)){$err="";}

    if(MODULE=="index"){
    $p->set_block(MODULE,"login_form");
        if($detect == "client"){
            $p->set_block("login_form", "client_auth");
            $user_type="client";
        } else {
            $p->set_block("login_form", "manager_auth");
            $p->set_block("login_form", "manager");
            $user_type="manager";

    }
    $p->set_var(array(
          "ERROR"             => $err,
          "FRM_LOGIN_ACTION"  => MAIN_URL,
          "LOGIN"             => $cook["login"],
          "CHALLENGE"         => $auth->sys["challenge"],
          "DOMAIN"            => $db_name,
          "USER_TYPE"         => $user_type,
      ));
    }
}
?>