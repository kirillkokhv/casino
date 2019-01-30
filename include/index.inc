<?php
$DB_name = explode(".", $_SERVER["SERVER_NAME"]);
$crm_account = $DB_name = $DB_name[0] == "tur1" ? "tur" : $DB_name[0];

if ($auth->CheckAuth()) {
	$p->set_var(array(
                "PAGE_HEADER" => "Рабочий стол",
                "PAGE_SUBHEADER" => "Подзаголовок",
   ));


}


?>