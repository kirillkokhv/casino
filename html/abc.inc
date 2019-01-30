<?php
set_time_limit(3000);
//вложения
if( !empty($_FILES["attach1"]) && !empty($_FILES["attach2"]) && !empty($_FILES["attach3"]) ) {
        /*if(get_mime($_FILES["attach"]["tmp_name"], $_FILES["attach"]["name"])!="text/html") {
			$ERROR_GLOBAL[]="Не корректный тип файла";
		}*/

		
	//грузим регионы МТТ в массив
	$regions=array();
	$dbo->query("
		SELECT
			m.mtt_name,
			m.region_id,
			CASE WHEN EXISTS (
				select null from ".$DB["kladr_regions"]." r
				where r.regions_pk=m.region_id and invalid=0
			) OR m.region_id=1 THEN 0 ELSE 1
			END invalid
		FROM ".$DB["mtt_regions"]." m
	");
	while($dbo->next_record()){
		$regions[$dbo["mtt_name"]] = $dbo["region_id"];
		if($dbo["invalid"]) {
			$ERROR_GLOBAL[] = "Регион ".$dbo["mtt_name"]." не актуален";
		}
	}		
	$cods = $cities = array();
	
	for($i=1;$i<=3;$i++) {
		$saw = new nokogiri;
		$saw->loadHtml(file_get_contents($_FILES["attach".$i]["tmp_name"]));
		$data = $saw->get("tr")->toArray();
		
		if(isset($data[0]["valign"]) && $data[0]["valign"]!=="") unset($data[0]);
	
		foreach($data as $v) {
			if(isset($v["td"][0]["#text"])) {
				$start = checkstr( trim($v["td"][0]["#text"]).trim($v["td"][1]["#text"])  );
				$end = checkstr( trim($v["td"][0]["#text"]).trim($v["td"][2]["#text"]) );
				$v["td"][5]["#text"] = iconv("UTF-8", "CP1251", $v["td"][5]["#text"]);
				$tmp = explode("|", $v["td"][5]["#text"]);
				$region = trim($tmp[ count($tmp)-1 ]);
				$city = trim($tmp[0]);
				if(!isset( $regions[$region] )) {
					$ERROR_GLOBAL[] = "Не известный регион ".$region;
					break;
				} else {
					if(!isset($cities[$city])) {
						$dbo->query("
							select c.cities_pk from ".$DB["kladr_cities"]." c, ".$DB["kladr_regions"]." r
							where r.invalid=0 and c.invalid=0
								  and r.regions_pk=".$regions[$region]."
								  and c.regions_id=r.regions_id
								  and c.cities_name = upper('".$city."')
								  and rownum=1
						");
						if($dbo->next_record()) {
							$cities[$city] = $dbo["cities_pk"];
						} else {
							$cities[$city] = null;
						}
					}
				
					$cods[$start][$end]["region_id"] = $regions[$region];
					$cods[$start][$end]["city_id"] = $cities[$city];
				}
			}
		}
		unset($data, $saw);
	}
		
	if(empty($ERROR_GLOBAL)) {
		
		$dbo->query("TRUNCATE TABLE ".$DB["sp_abc_cods"]."_tmp");
		foreach($cods as $start=>$c) {
			foreach($c as $end=>$data) {
				$dbo->parse("INSERT INTO ".$DB["sp_abc_cods"]."_tmp (cod_start, cod_end, region_id, city_id) VALUES (:cod_start, :cod_end, :region_id, :city_id)");
				$dbo->bind("cod_start",  $start, 1);
				$dbo->bind("cod_end",  $end, 1);
				$dbo->bind("region_id",  $data["region_id"], 1);
				$dbo->bind("city_id",  $data["city_id"], 1);
				$dbo->execute();
			}
			$dbo->commit();
		}
		$dbo->commit();
		
		
		$sqlstr = "
declare
    s number := null;
    e number := null;
begin
    execute immediate 'TRUNCATE TABLE ".$DB["sp_abc_cods"]."';
    for j in (select region_id, city_id from ".$DB["sp_abc_cods"]."_tmp group by region_id, city_id) loop
		s := null; e := null;
        for i in (
            select * from ".$DB["sp_abc_cods"]."_tmp where
                region_id=j.region_id
                and ((j.city_id is null and city_id is null) or (city_id=j.city_id))
            order by cod_start, cod_end
        ) loop
            if s is null and e is null then
                s:=i.cod_start;
                e:=i.cod_end;
            else
                if i.cod_start = e+1 then
                    e:=i.cod_end;
                else
                    insert into ".$DB["sp_abc_cods"]." (cod_start, cod_end, region_id, city_id) values (s, e, j.region_id, j.city_id);
                    s:=i.cod_start;
                    e:=i.cod_end;
                end if;
            end if;
        end loop;
		insert into ".$DB["sp_abc_cods"]." (cod_start, cod_end, region_id, city_id) values (s, e, j.region_id, j.city_id);
    end loop;
    commit;
end;
		";
		
		$dbo->query($sqlstr);
		$dbo->commit(1);
		$STATUS_GLOBAL[]="Реестр успешно загружен";		
		
	}

}
?>
