<?php
$DB_name = explode(".", $_SERVER["SERVER_NAME"]);
$crm_account = $DB_name = $DB_name[0] == "tur1" ? "tur" : $DB_name[0];

if ($auth->CheckAuth()) {
	$p->set_block(MODULE, "main");
	$p->set_var(array(
	                "PAGE_HEADER" => $lang["monitoring_header"],
	                "PAGE_SUBHEADER" => $lang["monitoring_subheader"],
	              	"MONITORING_HEADER" => $lang["monitoring_header"],
				    "MONITORING_SUBHEADER" => $lang["monitoring_subheader"],
				    "MONITORING_UID" => $lang["monitoring_uid"],
				    "MONITORING_LOGIN" => $lang["monitoring_login"],
				    "MONITORING_BALLS" => $lang["monitoring_balls"],
				    "MONITORING_STATUS" => $lang["monitoring_status"],
				    "MONITORING_LAST_PAY" => $lang["monitoring_last_pay"],
				    "MONITORING_RATING" => $lang["monitoring_rating"],
				    "MONITORING_IP" => $lang["monitoring_ip"],
				    "REFRESH_BUTTON" =>$lang["refresh_button"],
				    "AUTOREFRESH" =>$lang["autorefresh"],
				    "PLAYERS_BUTTONS" =>$lang["players_buttons"],



	                
	  ));
}
   

?>