<?php
set_vars(array("summa_rub","summa_usd","course","lname","fname","mname","pay_date","summa","detect","full_deal_summa","full_book",
"documents","full_deal_pays","full_deal_pays","full_pay","after_call","start_date","summa_rub","commentary","agent_summa_rub","agent_summa_usd","agent_course"), TYPE_STRING);
set_vars(array("id","deal_id","client_id","service_id","country_id","city_id","hotel_id","nights","operator_id","pay_type","room_type","rooms","food_type","place_type","type"), TYPE_INT);

if($detect=="edit_ok"){
    
    if(empty($ERROR_GLOBAL)){
        $sqlstr_u="";
		if($full_book=="on") $sqlstr_u.= ",full_book = 1";
		if($documents=="on") $sqlstr_u.= ",issued_docs = 1"; 
		if($full_pay=="on") $sqlstr_u.= ",full_payed = 1";
		if($after_call=="on") $sqlstr_u.= ",client_call = 1";
     
		$sqlstr="update deals set 
            full_summa = '".$full_deal_summa."',
            paid_summa = '".$full_deal_pays."'
			".$sqlstr_u."
			where deal_id = ".$deal_id;
        $dbo->query($sqlstr);
        
        $dbo->query("insert into deals_hist (deal_id,create_user_id, create_date,commentary)
            values (".$deal_id.", ".$auth->auth['uid'].",now(),'".iconv( "UTF-8","CP1251",$commentary)."')");
        echo "{success: true}";
        exit;
		
    } else {
        $detect="EDIT";
        $p->set_var(array(
                    "CLIENT_FNAME"           =>  ".$fname.",
                    "CLIENT_MNAME"           =>  ".$mname.",
                    "CLIENT_LNAME"           =>  ".$lname.",
                    "E_CLIENT_FNAME"           =>  ".$eng_fname.",
                    "E_CLIENT_LNAME"           =>  ".$eng_lname.",
                    "BIRTHDATE"           =>  ".$birthdate.",
                    "BIRTHPLACE"           =>  ".$birthplace.",
                    "E_BIRTHPLACE"           =>  ".$eng_birthplace.",
                    "WORK_PLACE"          => $work_lace,
                    "ADDRESS"          => $address,
                    "EMAIL"          => $email,
                    "URL"          => $url,
                    "ICQ"          => $icq,
                    "WORK_PHONE"          => $work_phone,
                    "MOB_PHONE"          => $mob_phone,
                    "HOME_PHONE"          => $home_phone,
                    "R_PASS_SER" => $r_pass_ser,
                    "R_PASS_NUM" => $r_pass_num,
                    "R_PASS_ISSUE_PLACE" => $r_pass_issue_place,
                    "R_PASS_ISSUE_DATE" => $r_pass_issue_date,
                    "R_PASS_END_DATE" => $r_pass_end_date,
                    "F_PASS_SER" => $f_pass_ser,
                    "F_PASS_NUM" => $f_pass_num,
                    "F_PASS_ISSUE_PLACE" => $f_pass_issue_place,
                    "F_PASS_ISSUE_DATE" => $f_pass_issue_date,
                    "F_PASS_END_DATE" => $f_pass_end_date,
                                
			
	));
    }
    
    
}

if($detect=="EDIT"&&!empty($id)){
    
	$p->set_block(MODULE,"client_form");
    $p->set_block("client_form","button_edit");
	$p->set_block("head","deal_info");
	$type=1;
	//$p->set_block("client_form","edit_deal");
	$p->set_block("client_form", "deal_hist", "hist");
    $dbo->query("select d.*, s.name status_name 
					
					from deals d, deals_statuses s 
					where deal_id =".$id." and d.status_id = s.status_id");
    if ($dbo->next_record()){
    
        $S_REACTIONS="";
        $dbo1->query("SELECT * from deals_reactions where status_in = ".$dbo["status_id"]);
    	while($dbo1->next_record()){
    		$S_REACTIONS.='<option value="'.$dbo1["reaction_id"].'"';
        	//if($rid==$p_reason_id) $S_OFFICES.=' selected';
        	$S_REACTIONS.='>'.$dbo1["reaction_name"].'</option>';
      	}
        if($type==1) $p->set_block("client_form","tour_block");
	    if($type==2) $p->set_block("client_form","hotel_block");
	    if($type==3) $p->set_block("client_form","ticket_block");
        $p->set_var(array(
		
                    "DEAL_ID"           =>  $id,
                    "CUSTIMER_ID"           =>  $dbo["customer_id"],
                    "FULL_SUMMA"           =>  $dbo["full_summa"],
                    "MANAGER_ID"           =>  $dbo["manager_id"],
                    "CREATE_DATE"           =>  $dbo["create_date"],
                    "SUMM_REST"           =>  $dbo["summ_rest"],
                    "START_DATE"           =>  $dbo["start_date"],
                    "END_DATE"           =>  $dbo["end_date"],
                    "DEAL_DATE"           =>  $dbo["deal_date"],
                    "PAY_TYPE_ID"          => $dbo["pay_type_id"],
                    "COMMENT"          => $dbo["comment"],
                    "OFFICE_ID"          => $dbo["office_id"],
                    "STATUS_ID"          => $dbo["status_id"],
					"STATUS_NAME"          => $dbo["status_name"],
                    "PAID_SUMMA"          => $dbo["paid_summa"],
                    "REST_SUMMA"          => $dbo["rest_summa"],
                    "OPERATOR_ID"          => $dbo["operator_id"],
                    "STATUS_NAME"       => $dbo["status_name"],
                    "STATUS_ID"         => $dbo["status_id"],
                    "REACTIONS"         => $S_REACTIONS,
					"FULL_BOOK"			=> $dbo["full_book"] == 1 ? "true" : "false",
					"FULL_PAYED"	   => $dbo["issued_docs"] == 1 ? "true" : "false",
					"DOCUMENTS"			=> $dbo["full_payed"] == 1 ? "true" : "false",
					"AFTER_CALL"			=> $dbo["client_call"] == 1 ? "true" : "false",
                
                                
			
	));
    	$dbo->query("SELECT d.commentary, d.create_date, concat(u.fname,' ',u.mname,' ',u.lname) man_fio from deals_hist d, auth_users u   
            where d.create_user_id = u.user_id 
            and deal_id = ".$id."
            order by d.create_date desc");
    	while($dbo->next_record()){
    		$p->set_var(array(
		
                    "MANAGER_FIO"           =>  $dbo["man_fio"],
                    "CREATE_DATE"           =>  $dbo["create_date"],
                    "COMMENTARY"           =>  $dbo["commentary"]
                    
            ));
            $p->parse("hist", "deal_hist", true);
      	}
    } 
    
}

if($detect=="ADD_OK"){
    //if(!$lname&&!$fname&&!$mname) $ERROR_GLOBAL[]="Не указаны ФИО";
    if(empty($ERROR_GLOBAL)){
        $sqlstr="insert into clients (
            fname,
            mname,
            lname,
            eng_fname,
            eng_lname,
            birthdate,
            birthplace,
            eng_birthplace,
            work_place,
            address,
            r_pass_ser,
            r_pass_num,
            r_pass_issue_place,
            r_pass_issue_date,
            f_pass_ser,
            f_pass_num,
            f_pass_issue_place,
            f_pass_issue_date,
            work_phone,
            mob_phone,
            home_phone,
            comment,
            sex,
            email,
            url
            )
        values (
            '".$fname."',
            '".$mname."',
            '".$lname."',
            '".$eng_fname."',
            '".$eng_lname."',
            '".$birthdate."',
            '".$birthplace."',
            '".$eng_birthplace."',
            '".$work_place."',
            '".$address."',
            '".$r_pass_ser."',
            '".$r_pass_num."',
            '".$r_pass_issue_place."',
            '".$r_pass_issue_date."',
            '".$f_pass_ser."',
            '".$f_pass_num."',
            '".$f_pass_issue_place."',
            '".$f_pass_issue_date."',
            '".$work_phone."',
            '".$mob_phone."',
            '".$home_phone."',
            '".$comment."',
            '".$sex."',
            '".$email."'
            '".$url."'
            )
        ";
        $dbo->query($sqlstr);
        $detect="";
    } else {
        $detect="ADD";
        $p->set_var(array(
                    "CLIENT_FNAME"           =>  ".$fname.",
                    "CLIENT_MNAME"           =>  ".$mname.",
                    "CLIENT_LNAME"           =>  ".$lname.",
                    "E_CLIENT_FNAME"           =>  ".$eng_fname.",
                    "E_CLIENT_LNAME"           =>  ".$eng_lname.",
                    "BIRTHDATE"           =>  ".$birthdate.",
                    "BIRTHPLACE"           =>  ".$birthplace.",
                    "E_BIRTHPLACE"           =>  ".$eng_birthplace.",
                    "WORK_PLACE"          => $work_lace,
                    "ADDRESS"          => $address,
                    "EMAIL"          => $email,
                    "URL"          => $url,
                    "ICQ"          => $icq,
                    "WORK_PHONE"          => $work_phone,
                    "MOB_PHONE"          => $mob_phone,
                    "HOME_PHONE"          => $home_phone,
                    "R_PASS_SER" => $r_pass_ser,
                    "R_PASS_NUM" => $r_pass_num,
                    "R_PASS_ISSUE_PLACE" => $r_pass_issue_place,
                    "R_PASS_ISSUE_DATE" => $r_pass_issue_date,
                    "R_PASS_END_DATE" => $r_pass_end_date,
                    "F_PASS_SER" => $f_pass_ser,
                    "F_PASS_NUM" => $f_pass_num,
                    "F_PASS_ISSUE_PLACE" => $f_pass_issue_place,
                    "F_PASS_ISSUE_DATE" => $f_pass_issue_date,
                    "F_PASS_END_DATE" => $f_pass_end_date,
                                
			
	));
    }
    
    
}

if($detect=="ADD"){
    $p->set_block(MODULE,"client_form");
    $p->set_block("client_form","button_add");
	if($type==1) $p->set_block("client_form","tour_block");
	if($type==2) $p->set_block("client_form","hotel_block");
	if($type==3) $p->set_block("client_form","ticket_block");
	//$p->set_block("client_form","new_deal");
    //$dbo->query("SELECT max(deal_id)+1 new_deal_id from deals");
	if (!isset($_SESSION["new_deal_id"])){
		$dbo->query("INSERT into deals (create_date, manager_id) values (current_date(),".$auth->auth['user_id'].")"); 
		if($dbo->last_id()){
			$new_deal_id = $dbo->last_id();
			$_SESSION["new_deal_id"] = $new_deal_id;
		}
	} else $new_deal_id = $_SESSION["new_deal_id"];
	
	$p->set_var(array(
						"DEAL_ID"           =>  $new_deal_id,
						"STATUS_NAME" 		=> "новый"
		));
}

if($detect=="" || $detect=="SEARCH" || $detect=="COUNT") {

	$p->set_block(MODULE, "form");


//c.fname || ' ' || c.mname || ' ' c.lname fio,
}

if ($detect=="client_grid"){
        $sqlstr="
    	SELECT
            d.deal_id,
            (select concat(c.fname,' ',c.mname,' ',c.lname) from clients where client_id = d.customer_id) customer_fio,
            (select count(1) from deals2clients dc where d.deal_id = dc.deal_id) client_number  
            (select concat(c.fname,' ',c.mname,' ',c.lname) from clients where client_id = d.manager_id) manager_fio,
            summ_rest,
            start_date,
            end_date,
            deal_date,
            paid_summa,
            rest_summa
            
        FROM deals d, tours t, countries c, hotels h
    	WHERE d.tour_id = t.tour_id
    	and d.country_id = c.countries_id
    	and d.hotel_id = h.hotel_id
    	
	";

	$sqlstr_w="";
		$dbo->query($sqlstr, ($p_page*10), 10);
	while($dbo->next_record()) {
         $ret[]= array(
            $dbo['client_id'], 
            $dbo["fio"],
            
            "Физ.лицо", 
            "ВИП",
            $dbo["sex"],
            $dbo['birthdate'], 
            $dbo["mob_phone"],
            $dbo["birthdate"],
            $dbo["f_pass_num"], 
            $dbo["r_pass_ser"],
            $dbo["r_pass_num"],
            $dbo['vip'] 
            
            );
    }
    echo "{success: true, data: ".json_encode($ret)."}";
    exit;        
      /*  while($dbo->next_record()) {
		$p->set_var(array(
                    "FIO"           => $dbo["fio"],
                    "FIO"           => $dbo["fio"],
                    "BIRTHDATE"          => $dbo["name"],
                    "PHONE"          => $dbo["name"],
                    "F_PASS"          => $dbo["name"],
                    "R_PASS"          => $dbo["name"],
                    
			
		));
		//$p->parse("l","list",true);
	}

	if($detect=="SEARCH") {
	  	unset($_SESSION["p_page_count_consults"]);
	}
	if($detect=="COUNT") {
  		$dbo->query("SELECT
    		count(1)
                
                
    	FROM tasks t
    	WHERE t.deleted = 0");
  		$dbo->next_record();
  		$_SESSION["p_page_count_consults"]=$dbo["count"];
	}

	if(isset($_SESSION["p_page_count_consults"])) {
		$p->set_var("NAVIGATION", pages($_SESSION["p_page_count_consults"], LEN, URI_URL));
		$p->set_var("NUM_ROWS", $_SESSION["p_page_count_consults"]);
	} else {
		$p->set_var("NAVIGATION", pages(LEN*3, LEN, URI_URL));
		$p->set_var("NUM_ROWS", '
			<a href="'.URI_URL.'?detect=COUNT&page=0" title="расчитать кол-во записей"><img src="'.IMG_URL.'quest.gif" width="16" height="16" align="absmiddle"></a>
		');
	}*/
}

if ($detect == "get_deal_clients_grid"){
    $sqlstr_w = '';
    /*foreach ($_SESSION["deal2client"] as $client_id=>$params){
        $dbo->query("SELECT
            c.client_id,
            concat(c.fname,' ',c.mname,' ',c.lname) fio,
            ".$params["payer"]." payer,
            ".$params["tourist"]." tourist
            FROM clients c
            WHERE client_id = ".$client_id);
        if ($dbo->next_record()){
            $ret[]= array(
            $dbo['client_id'], 
            $dbo["fio"],
            $dbo['payer'] 
            );
        }
        
    }*/
    $dbo->query("SELECT 
                    dc.deal_id,
                    c.client_id,
                    concat(c.fname,' ',c.mname,' ',c.lname) fio,
                    case when dc.payer = 1 then 'checked' else '' end payer,
                    case when dc.tourist = 1 then 'true' else 'false' end tourist,
                    case when vip = 1 then 'VIP' else '' end vip,
                    CASE WHEN sex = 1 THEN 'М' ELSE 'Ж' END sex,
                    c.birthdate,
                    c.mob_phone,
					concat(c.f_pass_ser, ' ',c.f_pass_num) f_pass,
					c.f_pass_end_date,
					c.email
				
				
                 FROM 
                    deals2clients dc, clients c 
                WHERE 
                    dc.deal_id =".$deal_id." 
                    and dc.client_id = c.client_id");
    while($dbo->next_record()) {
         $ret[]= array(
            $dbo['client_id'], 
            $dbo["fio"],
            $dbo['vip'], 
            $dbo["sex"],
            $dbo['birthdate'], 
            $dbo["tourist"],
            $dbo["payer"],
            $dbo["mob_phone"],
			$dbo["f_pass"],
			$dbo["f_pass_end_date"],
			$dbo["email"]
           
            
            );
    }
    echo "{success: true, data: ".json_encode($ret)."}";
    exit;        

}

if ($detect == "get_deals_grid"){
    
	
	$dbo->query("SELECT d.deal_id,
				   d.create_date,
				   (SELECT concat(c.fname,' ',c.mname,' ',c.lname)
					  FROM clients c
					 WHERE c.client_id = d.customer_id)
					  customer_fio,
				   d.start_date,
				   d.full_summa,
				   d.rest_summa,
				   (SELECT name
					  FROM operators o
					 WHERE o.operator_id = d.operator_id)
					  operator,
					ds.name status_name,
                    issued_docs
				FROM deals d, deals_statuses ds
				WHERE manager_id =".$auth->auth["user_id"]."
				and d.status_id = ds.status_id"
				);
    
    while($dbo->next_record()) {
         $ret[]= array(
            $dbo['deal_id'], 
            $dbo["customer_fio"],
           	$dbo["create_date"],
            $dbo['full_summa'],
			$dbo["status_name"],
            $dbo["issued_docs"] == 1 ? "true" : "false",
		
           
            
            );
    }
   
	echo "{success: true, data: ".json_encode($ret)."}";
    exit;        

}

if ($detect == "get_deals_services_grid"){
    $sqlstr ="select 
                s.service_id, s.service_type_id, s.service_cost_rub, s.service_cost_usd, s.service_rest_usd, s.service_agent_rest_usd,
                s.status_id, s.deal_id, 1, s.service_payed_usd, s.service_agent_payed_usd, s.service_agent_cost_usd,
                (select name from countries c where c.country_id = s.service_country_id) country_name,
                t.*
                
                    
                    
            

             from
            services s,
            deals2tours t 
           /* (SELECT id service_id,
            				'Отель ' service_type,
            				2 service_type_id,
            				concat((SELECT name
            							 FROM hotels h
            							WHERE h.hotel_id = dh.hotel_id),
            						  ' c ',
            						  check_in_date,
            						  ' на ',
            						  nights,
            						  ' ночи/ночей')
            				service_name,
            				summa_rub,
            				summa_usd,
            				course,
            				status_id,
            				rest
            				FROM deals2hotels dh
            				
            				UNION ALL
            				SELECT tour_id service_id,
            				'Тур ' service_type,
            				1 service_type_id,
            				name,
            				summa_rub,
            				summa_usd,
            				course,
            				status_id,
            				rest
            				FROM deals2tours dt
            				
            				UNION ALL
            				SELECT ticket_id,
            				   'Билет',
            				   3,
            				   concat(departure_date,
            						  ' из ',
            						  departure_city,
            						  ' в ',
            						  arrival_city), 
            				summa_rub,
            				summa_usd,
            				course,
            				status_id,
            				rest
            			  FROM deals2tickets dt
            			  ) ss*/
            
            where s.service_id = t.tour_id
            and s.deal_id = ".$deal_id;
	
	$dbo->query($sqlstr);			  
			  
			  
			  
    
    while($dbo->next_record()) {
        
         $ret[]= array(
            $dbo['service_id'], 
			//$dbo["service_type"],
            $dbo["service_type"],
			$dbo['service_type_id'], 
            //iconv("CP1251", "UTF-8",$dbo["service_name"]),   
			$dbo["name"],
            $dbo["service_cost_rub"],
			$dbo["service_cost_usd"],
			$dbo["course"],
			$dbo["service_cost_usd"],
			$dbo["service_rest_usd"],
			$dbo["service_agent_rest_usd"],
			$dbo["status_id"],
			$dbo["service_payed_usd"],
			$dbo["service_agent_payed_usd"],
			$dbo["country_name"],
			$dbo["service_agent_cost_usd"],
			$dbo["country_id"],
			$dbo["city_id"],
			$dbo["hotel_id"],
			$dbo["start_date"],
			$dbo["nights"],
			$dbo["operator_id"],
			$dbo["end_date"]
			
			
           
            
            );
    }
   
	echo "{success: true, data: ".json_encode($ret)."}";
    exit;        

}

if($detect=="CLIENT_ADD" && $client_id>0 and $deal_id>0){
//$_SESSION["deal2client"][] = $client_id;

        $dbo->query("
            INSERT INTO deals2clients 
                (deal_id,client_id) 
                VALUES
                (".$deal_id.",".$client_id.")");
        echo "{success: true}";
        exit;
}

if($detect=="del_client_from_deal" && $deal_id > 0 and $client_id >0){
	$dbo->query("
		delete from deals2clients where deal_id = ".$deal_id." and client_id = ".$client_id);
	echo "{success: true}";
        exit;
}

if($detect=="get_deals_pays_grid" && $deal_id > 0){
	//$dbo->query("select d2p.* from deals2pays d2p, services s where d2p.service_id = s.service_id");
	$dbo->query("
		select 
			d2p.pay_id,
			s.service_id,
			s.service_name,
			d2p.summa_rub,
			d2p.course,
			d2p.summa_usd,
			d2p.create_date,
			case when d2p.pay_type = 1 then 'От туриста' when  d2p.pay_type = 2 then 'Контрагенту' end pay_type,
			pay_type pay_type_id
				from 
			deals2pays d2p,
			(SELECT id service_id,
				'Отель ' service_type,
				2 service_type_id,
				concat((SELECT name
							 FROM hotels h
							WHERE h.hotel_id = dh.hotel_id),
						  ' c ',
						  check_in_date,
						  ' на ',
						  nights,
						  ' ночи/ночей')
				service_name,
				summa_rub,
				summa_usd,
				course
			  FROM deals2hotels dh
			  where dh.deal_id = ".$deal_id."
			UNION ALL
			SELECT ticket_id,
				   'Билет',
				   3,
				   concat(departure_date,
						  ' из ',
						  departure_city,
						  ' в ',
						  arrival_city), 
					summa_rub,
					summa_usd,
					course
			  FROM deals2tickets dt
			  where dt.deal_id = ".$deal_id."
			  UNION ALL
				SELECT tour_id,
					   'Тур',
					   1,
					   name, 
						summa_rub,
						summa_usd,
						course
				  FROM deals2tours dt
				  where dt.deal_id = ".$deal_id.") s
			  where d2p.service_id = s.service_id
	
	");
	while($dbo->next_record()) {
         
		 $ret[]= array(
            $dbo['pay_id'], 
			$dbo['service_id'], 
			//iconv("CP1251", "UTF-8",$dbo['service_name']),
			$dbo['service_name'],
			$dbo["summa_usd"],
			$dbo["summa_rub"],
			$dbo["course"],
			$dbo["create_date"],
			$dbo['pay_type'],
			$dbo["pay_type_id"]
           
            
            );
    }
	echo "{success: true, data: ".json_encode($ret)."}";
    exit;        
	
}

if($detect=="pay_add" && $deal_id > 0 && $service_id > 0){
	if($dbo->query("
		insert into deals2pays (deal_id, service_id, summa_rub, summa_usd, course, create_date, pay_type)
		values (".$deal_id.", ".$service_id.", ".$summa_rub.",".$summa_rub/$course.",".$course.",curdate(), ".$pay_type.") 
	"))
	{
		//$dbo->query("update ");
		echo "{success: true, data: ".json_encode($ret)."}";
	}
	else echo "{success: false, error:'ошибка'}";
    exit;  
	
}

if($detect=="get_countries"){
	$dbo->query("select * from countries");
	while($dbo->next_record()){
		$ret[]=array(
			"country_id" => $dbo["country_id"],
			"name" => $dbo["name"]
		);
	}	
	echo "{success: true, data: ".json_encode($ret)."}";
	exit;
}

if($detect=="get_cities" && $country_id > 0){
	$dbo->query("select * from cities where country_id=".$country_id);
	while($dbo->next_record()){
		$ret[]=array(
			//"country_id" => $dbo["country_id"],
			"city_id" => $dbo["city_id"],
			"city_name" => $dbo["city_name"]
		);
	}	
	echo "{success: true, data: ".json_encode($ret)."}";
	exit;
}

if($detect=="get_hotels" && $city_id > 0){
	$dbo->query("select * from hotels where city_id=".$city_id);
	while($dbo->next_record()){
		$ret[]=array(
			//"country_id" => $dbo["country_id"],
			"hotel_id" => $dbo["hotel_id"],
			"hotel_name" => $dbo["name"]
		);
	}	
	echo "{success: true, data: ".json_encode($ret)."}";
	exit;
}

if($detect=="get_operators"){
	$dbo->query("select * from operators where deleted = 0");
	while($dbo->next_record()){
		$ret[]=array(
			//"country_id" => $dbo["country_id"],
			"operator_id" => $dbo["operator_id"],
			"operator_name" => $dbo["name"]
		);
	}	
	echo "{success: true, data: ".json_encode($ret)."}";
	exit;
}
/*
if($detect=="get_operators"){
	$dbo->query("select * from operators where deleted = 0");
	while($dbo->next_record()){
		$ret[]=array(
			//"country_id" => $dbo["country_id"],
			"operator_id" => $dbo["operator_id"],
			"operator_name" => $dbo["name"])
		);
	}	
	echo "{success: true, data: ".json_encode($ret)."}";
	exit;
}*/


if($detect=="tour_add_ok" && $deal_id > 0){
	$dbo->query("select * from hotels where hotel_id =".$hotel_id);
	if($dbo->next_record()) $hotel_name = $dbo["name"].' '.$dbo["stars"].'* ('.$nights.' nights)';
	
	$dbo->query("insert into services (service_type_id, service_cost_rub, course, service_cost_usd, service_agent_cost_rub, agent_course, service_agent_cost_usd, service_rest_usd, service_agent_rest_usd, status_id, deal_id, service_country_id)
	values(1,'".$summa_rub."','".$course."', '".$summa_rub/$course."','".$agent_summa_rub."','".$agent_course."', '".$agent_summa_rub/$agent_course."','".$summa_rub/$course."','".$agent_summa_rub/$agent_course."',1,".$deal_id.",".$country_id.")");
	$service_id = $dbo->last_id();
	
    if($service_id){
    
        if($dbo->query("
    		insert into deals2tours (tour_id,deal_id,  name, country_id, city_id, hotel_id, start_date, nights, operator_id, end_date, summa_rub, course, summa_usd, status_id, rest, create_date, create_user_id,rooms,room_type, food_type, place_type)
    		values (".$service_id.",".$deal_id.", '".$hotel_name."', ".$country_id.",".$city_id.",".$hotel_id.",'".$start_date."',".$nights.",".$operator_id.",'".$start_date."','".$summa_rub."','".$course."', '".$summa_rub/$course."', 1, '".$summa_rub/$course."', curdate(),".$auth->auth["uid"].",".$rooms.",".$room_type.",".$food_type.",".$place_type.") 
    	"))
    	{
    		//$dbo->query("update ");
    		echo "{success: true}";
    	}
    }
	else echo "{success: false, error:'ошибка'}";
    exit;  
	
}

if($detect=="tour_edit_ok" && $service_id > 0 ){
	$dbo->query("select * from hotels where hotel_id =".$hotel_id);
	if($dbo->next_record()) $hotel_name = $dbo["name"].' '.$dbo["stars"].'* ('.$nights.' nights)';
	
	$dbo->query("insert into services (service_type_id, service_cost_rub, course, service_cost_usd, service_agent_cost_rub, agent_course, service_agent_cost_usd, service_rest_usd, service_agent_rest_usd, status_id, deal_id, service_country_id)
	values(1,'".$summa_rub."','".$course."', '".$summa_rub/$course."','".$agent_summa_rub."','".$agent_course."', '".$agent_summa_rub/$agent_course."','".$summa_rub/$course."','".$agent_summa_rub/$agent_course."',1,".$deal_id.",".$country_id.")");
	$service_id = $dbo->last_id();
	
    if($service_id){
    
        if($dbo->query("
    		insert into deals2tours (tour_id,deal_id,  name, country_id, city_id, hotel_id, start_date, nights, operator_id, end_date, summa_rub, course, summa_usd, status_id, rest, create_date, create_user_id,rooms,room_type, food_type, place_type)
    		values (".$service_id.",".$deal_id.", '".$hotel_name."', ".$country_id.",".$city_id.",".$hotel_id.",'".$start_date."',".$nights.",".$operator_id.",'".$start_date."','".$summa_rub."','".$course."', '".$summa_rub/$course."', 1, '".$summa_rub/$course."', curdate(),".$auth->auth["uid"].",".$rooms.",".$room_type.",".$food_type.",".$place_type.") 
    	"))
    	{
    		//$dbo->query("update ");
    		echo "{success: true}";
    	}
    }
	else echo "{success: false, error:'ошибка'}";
    exit;  
	
}


if($detect=="set_tourist" &&$deal_id > 0 && $client_id >0)
{
    $tr=0;
    if($tourist) $tr=1;
    if ($dbo->query("update deals2clients set tourist =".$tr." where deal_id = ".$deal_id." and client_id = ".$client_id))
    echo "{success:true}";
    exit;
    

}

if($detect=="set_payer" && $deal_id > 0 && $client_id >0)
{   
    
    $dbo->query("update deals2clients set payer=0 where deal_id = ".$deal_id);
    if ($dbo->query("update deals2clients set payer = 1 where deal_id = ".$deal_id." and client_id = ".$client_id))
	
	$dbo->query("update deals set customer_id = ".$client_id." where deal_id = ".$deal_id);
    echo "{success:true}";
    exit;
    

}

if($detect=="change_service_status" && $deal_id > 0 && $service_id >0 && $status_id > 0)
{   
    
    $dbo->query("update services set status_id=".$status_id." where service_id = ".$service_id);
    echo "{success:true}";
    exit;
    

}
/*
function check_deal_status($deal_id){
    $dbo->("select * from deals2clients where deal_id = ".$deal_id);
    if ($dbo->next_record()) $new_status_id = 2;
    $dbo->("select * from deals2services where deal_id = ".$deal_id);
    while($dbo->next_record()){
        switch ($dbo["status_id"])){
            case 1: $status_id = 1; break;
            case 1: $status_id = 1; break;
            case 1: $status_id = 1; break;
            case 1: $status_id = 1; break;
        
        }
    }        

}*/

if($detect=="print_document" && $deal_id > 0){

    $dbo->query("select d.deal_id, c.lname, now() deal_date, d.full_summa, d2c.tourist, d2c.payer from deals d, deals2clients d2c, clients c
    where d.deal_id = d2c.deal_id
    and d2c.client_id = c.client_id
    ");
    $dbo->next_record();
        
    
    require_once LIBS. 'PHPWord.inc';

    $PHPWord = new PHPWord();
    //echo mb_detect_encoding ( $auth->auth["fio"]);exit;
    $template_name="test.docx";
    //$str = mb_convert_encoding('фывфыв', "UTF-8", "WINDOWS-1251");
    //echo $str; exit; 
    //echo MAIN_PATH.'files/'.$template_name; exit;
    $document = $PHPWord->loadTemplate(MAIN_PATH.'files/'.$template_name);
    $document->setValue('${manager}',  $auth->auth["fio"]);
    $document->setValue('${doc_number}',  $dbo["deal_id"]);
    $document->setValue('${client_fio}',  $dbo["lname"]);
    $document->setValue('${deal_summ}',  $dbo["full_summa"]);
    $document->setValue('${cur_date}',  $dbo["deal_date"]);
    $document->setValue('$cur_date',  $dbo["deal_date"]);
    //print_r($document); exit;
    
    
    $file_name='tmp_'.date(dms);
    $document->save(GLOBAL_PATH.'temp/'.$file_name.'.docx');
        
    header('Content-Disposition: attachment;filename="'.$file_name.'.docx"');
    header('Cache-Control: max-age=0');
        
    echo file_get_contents(GLOBAL_PATH.'temp/'.$file_name.'.docx');
    exit;
   
     
}
							

?>
