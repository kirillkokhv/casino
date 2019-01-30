<?php
$DB_name = explode(".", $_SERVER["SERVER_NAME"]);
$crm_account = $DB_name = $DB_name[0] == "tur1" ? "tur" : $DB_name[0];

if ($auth->CheckAuth()) {

//    $p->set_block("head", "header");
    $p->set_block(MODULE, "main");
    //print_r($auth->auth);
    if ($auth->auth["agency"]["agency_id"] == 1) $p->set_block("main", "request_info");


}


?>