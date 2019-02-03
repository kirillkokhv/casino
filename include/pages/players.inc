<?php
$DB_name = explode(".", $_SERVER["SERVER_NAME"]);
$crm_account = $DB_name = $DB_name[0] == "tur1" ? "tur" : $DB_name[0];
if($detect==""){

	$p->set_block(MODULE, "main");
	$p->set_block("main","list","l");
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
	    "TOTAL" =>$lang["total"],
	    "FILTER_BUTTON" =>$lang["filter"]
                
	  ));
   
	$dbo->query("SELECT * FROM accounts a
					LEFT JOIN balance b
					ON a.id = b.id");
	while($dbo->next_record()){
		$p->set_var(array(
            "ROW_NUM"          => $i++,
            "LOGIN"            => $dbo["name"],
            "BALLS"        	   => $dbo["CRD"],
            "LAST_PAY_DATE"    => $dbo["LAST_PAY_DATE"],
            "LAST_PAY_DATE" 	=> $dbo["LAST_PAY_DATE"],
            "RATING"         	=> $dbo["RATING"],
            "IP"    			=> $dbo["ip"],
            "STATUS"       => "offline",
          
                    ));
        $p->parse("l", "list", true);
	}
	$p->set_var(array(
		"PLAYERS_TOTAL"            => $dbo->num_rows(),
        "PLAYERS_ONLINE"           => "3",	
        "PLAYERS_BLOCKED"		   => "1",
        "PLAYERS_HIGH_RATING" 	   => "5",
        "PLAYERS_WITH_PAYS"       => "2",
        "PAYS_TOTAL" => "3",
        "PAYED_PLAYER" => "1",
        "PLAYERS_BUY_RATING" => "1"


	));


}
   

?>