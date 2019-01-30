if($detect == "get_tours_count"){
    
        $db->query("select 
                        ifnull((select count(Distinct  d.deal_id) start_deals from deals d, deals2clients dc where d.deal_id= dc.deal_id and start_date = '".$date."' group by d.deal_id),0) start_deals,
                    
                    ifnull((select 
                    count(dc.client_id) start_clients
                    from deals d,
                    deals2clients dc
                    where d.deal_id= dc.deal_id
                     and start_date = '".$date."'
                    group by d.deal_id),0) start_clients,
                    
                    ifnull((select 
                    count(Distinct  d.deal_id) end_deals
                    from deals d,
                    deals2clients dc
                    where d.deal_id= dc.deal_id
                    and end_date = '".$date."'
                    group by d.deal_id),0) end_deals,
                    
                    ifnull((select 
                     count(dc.client_id) end_clients
                    from deals d,
                    deals2clients dc
                    where d.deal_id= dc.deal_id
                    and end_date = '".$date."'
                    group by d.deal_id),0) end_clients
    
    
        ");
     if($db->next_record()){;
        echo json_encode(array(
    	   'start_deals'=>$db["start_deals"], 
           'start_clients'=>$db["start_clients"],
           'end_deals'=>$db["end_deals"],
           'end_clients'=>$db["end_clients"],
           'start_deals'=>$db["start_deals"],
           'start_deals'=>$db["start_deals"]
      	));
    }

    exit;        
        
        	
}