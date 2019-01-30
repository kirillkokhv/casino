<?php

$TIME_PHP_HEAD = microtime(1);
$TIME_SQL = 0;

$ERROR_GLOBAL = $STATUS_GLOBAL = array(); //Массив глобальных ошибок и статусов
error_reporting(E_ALL);
require_once "inc.inc";


### ОТОБРАЖЕНИЕ ОШИБОК ###
if (IS_ADMIN) {
    error_reporting(E_ALL | E_STRICT);
    error_reporting(E_ALL);
    error_reporting(7);
    ini_set("display_errors", 1);
    ini_set("html_errors", 0);
    ini_set("error_prepend_string", "PHP_ERRORS[]='");
    ini_set("error_append_string", "';");
} else {
    ini_set("display_errors", 0);
    error_reporting(E_ALL & ~E_DEPRECATED);
}
ini_set("zlib.output_compression", 1);
ini_set("zlib.output_compression_level", 2);
//import_request_variables("GPCS");
### ЗАПУСК КЛАССОВ СЕССИИ И АВТОРИЗАЦИИ ###

$sess = new Session;
if (!isset($_SESSION["auth"])) {
    $auth = new Auth;
    $auth->ObjectName = "auth";
    $auth->SessName = "sess";
    $auth->start();
} else {
    $auth = $_SESSION["auth"];
}
$auth->CheckLifeTime();
if (isset($_SESSION["cook"])) {
    $cook = $_SESSION["cook"];
} else {
    $cook = new Cookie;
}


### БД ###

$server = explode(".", $_SERVER["SERVER_NAME"]);

if (count($server) > 1) $db_name = $server[0];
else $db_name ="";



if (!isset($db)) {
    $db = new DB_MySQL(array("ShowErr" => IS_ADMIN));
} else {
    $db->clear();
}

//$db_name="forpost";
if ($db_name != "") {
    $db->query("select * from agency_info where db_name ='" . $db_name . "'");
    if ($db->next_record()) {
        $connect_param = array(
            "Database" => DB_PREFIX . "_" . $db_name,
            "User" => DB_PREFIX . "_" . $db["db_user"],
            "Password" => $db["db_pass"],
        );
        $auth->auth["agency"]["agency_id"] = $db["agency_id"];
    }
}

if (!isset($dbo)) {
    $dbo = new DB_MySQL($connect_param);
} else {
    $dbo->clear();
}
if (!isset($dbu)) {
    $connect_param["SessArgs"] = "character_set_results = 'utf8', character_set_client = 'utf8', character_set_connection = 'utf8', character_set_database = 'utf8', character_set_server = 'utf8'";
    $dbu = new DB_MySQL($connect_param);
} else {
    $dbu->clear();
}


### КОНСТАНТЫ ###
/* if(empty($_SESSION["constants"])) {
  $_SESSION["constants"]=array();
  $dbo->query("SELECT name, value, type FROM sp_constant"]);
  while($dbo->next_record()) {
  if(!defined($dbo["name"])) {
  if($dbo["type"]==1) $dbo["value"]=intval($dbo["value"]);
  $_SESSION["constants"][$dbo["name"]]=$dbo["value"];
  define($dbo["name"], $dbo["value"],0);
  }
  }
  } else {
  foreach($_SESSION["constants"] as $n=>$v) {
  if(!defined($n)) define($n, $v,0);
  }
  }

 */


require_once(INC_PATH . "user.inc");

define("URI", preg_replace("/^\/|\/[&?].*$|\/$/", "", $_SERVER["REQUEST_URI"]) . "/", 0);
//define("URI", preg_replace("/\?.*$/", "",  ($_SERVER["REQUEST_URI"]=="/") ? "" : $_SERVER["REQUEST_URI"]   ), 0);
define("URI_URL", MAIN_URL . URI, 0);

$TITLE = "";
### ПРОВЕРКА АВТОРИЗАЦИИ ###

if ($deep1 == "api") {

    set_vars(array("client_card"), TYPE_INT);
    set_vars(array("dr", "type", "json"), TYPE_STRING);
    /*  $json = '{
      "type": "confirmation",
      "fromUser": {
          "card": "111222"
      },
      "toUser": {
          "card": "111222",
          "name": "Иванов"
      }
  }';*/
    $json = base64_decode($json);
    $data = json_decode($json, true);
//print_r($data);
    //$client_card = 100000;
    if ($data["type"] == "request") {
        $dbo->query("select * from clients where card_num =" . $data["fromUser"]["card"]);
        if ($dbo->next_record()) {
            $dbo->query("select * from clients where card_num=" . $data["toUser"]["card"] . " and lname='" . $data["toUser"]["name"] . "'");
            if ($dbo->next_record()) {
                if ($dbo["parent_card_num"] == 100000)
                    $response["status"] = "success";
                else {
                    $response["status"] = "failure";
                    $response["description"] = "Запрашиваемый клиент уже привязан";
                }
            } else {
                $response["status"] = "failure";
                $response["description"] = "Не найден клиент запросивший перепривязку";
            }
        } else {
            $response["status"] = "failure";
            $response["description"] = "Не найден клиент для перепривязки";
        }
        $json = json_encode($response);
//print_r($response);
        echo $json;
        exit;
    }
    if ($data["type"] == "confirmation") {
        $dbo->query("select * from clients where card_num =" . $data["fromUser"]["card"]);
        if ($dbo->next_record()) {
            $dbo->query("select * from clients where card_num='" . $data["toUser"]["card"] . "' and lname='" . $data["toUser"]["name"] . "'");
            if ($dbo->next_record()) {
                if ($dbo["parent_card_num"] == 100000) {
                    $client_id = $dbo["client_id"];
                    $dbo->query("update clients set parent_card_num =" . $data["fromUser"]["card"] . " where client_id=" . $client_id);
                    $response["status"] = "success";
                } else {
                    $response["status"] = "failure";
                    $response["description"] = "Запрашиваемый клиент уже привязан";
                }
            } else {
                $response["status"] = "failure";
                $response["description"] = "Не найден клиент запросивший перепривязку";
            }
        } else {
            $response["status"] = "failure";
            $response["description"] = "Не найден клиент для перепривязки";
        }
        $json = json_encode($response);
        //print_r($response);
        echo $json;
        exit;
    }
    if ($client_card > 0) {
        if ($dr != "") {


            $dbo->query("select * from clients where card_num =" . $client_card . " and birthdate = STR_TO_DATE(  '" . $dr . "', '%d.%m.%Y')");
            if ($dbo->next_record()) {


                $response["waiting_ball"]["sum"] = 0;
                $response["client_ball"]["sum"] = 0;
                $dbo->query("SELECT 
              b.client_id,
              DATE_FORMAT(b.create_date, '%d.%m.%Y') create_date,
              SUM(b.balls) balls,
              b.status_id,
              dp.deal_id
             
            FROM
              clients_referal_balls b,
              deals2pays dp,
              clients c
            WHERE b.client_id = c.client_id and c.card_num = " . $client_card . " AND b.pay_id = dp.pay_id and b.status_id = 2 
            GROUP BY DATE_FORMAT(b.create_date, '%d.%m.%Y'),
              b.client_id,
              b.status_id,
              dp.deal_id");

                if ($dbo->num_rows() > 0) {
                    $response["status"] = "success";
                    $response["client_card"] = $client_card;


                }
                while ($dbo->next_record()) {
                    $response["client_ball"][$dbo["deal_id"]]["create_date"] = $dbo["create_date"];
                    $response["client_ball"][$dbo["deal_id"]]["balls"] = $dbo["balls"];
                    $response["client_ball"]["sum"] += $dbo["balls"];

                }

                $dbo->query("SELECT 
              b.client_id,
              DATE_FORMAT(b.create_date, '%d.%m.%Y') create_date,
              SUM(b.balls) balls,
              b.status_id,
              dp.deal_id,
              DATE_FORMAT(adddate(b.create_date, interval 2 month), '%d.%m.%Y') apply_date
            FROM
              clients_referal_balls b,
              deals2pays dp,
              clients c
            WHERE b.client_id = c.client_id and c.card_num = " . $client_card . " AND b.pay_id = dp.pay_id and b.status_id = 1 
            GROUP BY DATE_FORMAT(b.create_date, '%d.%m.%Y'),
              b.client_id,
              b.status_id,
              dp.deal_id");

                if ($dbo->num_rows() > 0) {
                    $response["status"] = "success";
                    $response["client_card"] = $client_card;
                    $response["waiting_ball"]["sum"] = 0;

                }
                while ($dbo->next_record()) {
                    $response["waiting_ball"][$dbo["deal_id"]]["create_date"] = $dbo["create_date"];
                    $response["waiting_ball"][$dbo["deal_id"]]["balls"] = $dbo["balls"];
                    $response["waiting_ball"][$dbo["deal_id"]]["apply_date"] = $dbo["apply_date"];
                    $response["waiting_ball"]["sum"] += $dbo["balls"];

                }

                getClientTree($client_card, 0, null, 1);

                $referal_json .= "}";

                $response = $response + json_decode($referal_json, true);

                $json = json_encode($response);

                echo $json;
                // print_r(json_decode(htmlspecialchars_decode($json), true)) ;
                exit;
            } else {
                echo '{
    "status":"failed","code":"1","description":"Не найден клиент по указанным условиям"}';
                exit;
            }
        } else {
            echo '{
    "status":"failed","code":"2","description":"Не указана дата рождения"}';
            exit;
        }
    } else {
        echo '{
    "status":"failed","code":"3","description":"Не указан номер карты"}';
        exit;
    }
}

if ($auth->CheckAuth()) {
    if (isset($db))
        $db1 = clone $db;
    if (isset($dbo)) {
        $dbo1 = clone $dbo;
        $dbo2 = clone $dbo;
        $dbo3 = clone $dbo;
    }

    //авторизован
    ####### Если требуется смена пароля
    if (!empty($reason_pass)) {
        $deep1 = "personal";
        $deep2 = "pass";
    }
    if ($auth->auth["user_type"] == "client") {
        $deep1 = "clients";
        $deep2 = "personal";
    }
    ####### Проверка разделов и прав доступа
    /* if(isset($auth->auth["access_menu"])) {
      //print_r($auth->auth["access_menu"]);exit;
      # Проверка прав пользователя
      if( array_key_exists(URI, $auth->auth["access_menu"])){
      $TITLE=$auth->auth["access_menu"][URI]["title"];
      } elseif($deep1=="index"){
      $TITLE="Выберите раздел";
      } else {
      $deep1="error"; $deep2="403";
      }
      } */
    $menu_tree = "
		<li><a href={MAIN_URL}clients/clientlist/>Клиенты </a>
		<li><a href={MAIN_URL}orders/orderlist/>Сделки </a>
		<li><a href={MAIN_URL}admin/mails/>Почта </a> 
		";
    //if($auth->auth["admin"]==1){
    $menu_tree .= "
			<li><a href={MAIN_URL}admin/pay_confirm/>Подтверждение платежей </a>
			<li><a href={MAIN_URL}admin/hotels/>Справочники</a>
			<li><a href={MAIN_URL}admin/users/>Пользователи</a>
			";

    //}
} else {
    $auth->auth["fio"] = "нет авторизации";
    $deep1 = "index";
}
#######
##### РАЗДЕЛЫ САЙТА

if ($_GET["service"] == "json") {
    set_time_limit(0);
    $dbo->query("select * from cities_temp c where not exists(select * from hotels_temp ct where ct.city_id = c.city_id)");
    //$dbo->query("select * from countries_temp c where not exists(select * from cities_temp ct where ct.country_id = c.country_id)");
    $i = 0;
    $cnt = 0;
    while ($dbo->next_record()) {


        $url = file_get_contents("http://module.sletat.ru/Main.svc/GetHotels?countryId=" . $dbo["country_id"] . "&towns=" . $dbo["city_id"] . "&stars=&all=-1");

        $content = json_decode($url, true);

        /*print_r($content["GetCountriesResult"]["Data"]);
        exit;*/
        //print_r($content);
        //exit;
        /*
         * Страны
            foreach ($content["GetCountriesResult"]["Data"] as $key => $countries) {

                if($countries["Id"]>0){
                    $dbo->query("insert into countries_temp
                    (country_id, name, isvisa)
                    values ('" . $countries["Id"] . "','" . $countries["Name"] . "','" . $countries["IsVisa"] . "')");

                }

            }
            }
        */
        /*
                // Города
                foreach ($content["GetCitiesResult"]["Data"] as $key => $cities) {

                    if ($cities["Id"] > 0) {
                        $dbo1->query("insert into cities_temp
                    (city_id,country_id, name, descriptionurl)
                    values ('" . $cities["Id"] . "','" . $cities["CountryId"] . "','" . $cities["Name"] . "','" . $countries["DescriptionUrl"] . "')");

                    }


                }*/
        // Отели
        foreach ($content["GetHotelsResult"]["Data"] as $key => $hotels) {

            if ($hotels["Id"] > 0) {

                $hotel_name = str_replace("'", "", $hotels["Name"]);
                $dbo1->query("insert into hotels_temp
(hotel_id, city_id, country_id, name, beach_line, stars)
            values('" . $hotels["Id"] . "', '" . $hotels["TownId"] . "', '" . $dbo["country_id"] . "', '" . $hotel_name . "', '" . $hotels["BeachLineId"] . "', '" . $hotels["StarId"] . "')");

            }


        }
        $cnt++;
        echo $cnt;
        $i++;
        if ($i == 100) header("Location: http://turnd/?service=json");


    }


    exit;
}


if (!isset($auth->auth["attr"]["service_id"]))
    $auth->auth["attr"]["service_id"] = 0;

$p = new Template(HTML_PATH, "remove");
//Постоянные шаблоны
$p->set_file(array(
                 "head" => "_head.ht", //Шапка
                 "foot" => "_foot.ht", //Низ
                 "main_top" => "_main_top.ht",
                 "main_bottom" => "_main_bottom.ht",
                 "error" => "_error.ht",
                 "status" => "_status.ht",
             ));


$module = null;
if($auth->CheckAuth()){


    switch ($deep1) {
        default:  //ошибка
            $deep2 = 404;
            define("MODULE", "error");
            break;

        case "index":  //главная
            define("MODULE", "index");
            break;


        case "close":  //закрываем страницу
            define("MODULE", "close");
            break;


        case "tasks":
            switch ($deep2) {
                case "tasklist":
                    define("MODULE", "tasks/tasklist");
                    break;
                default:
                    define("MODULE", "error");
                    break;
            }
            break;

        case "deals":
            switch ($deep2) {
                case "deals":
                    define("MODULE", "deals/deals");
                    break;
               
                default:
                    define("MODULE", "error");
                    break;
            }
            break;


        case "clients":
            switch ($deep2) {
                case "personal":  //главная
                    define("MODULE", "clients/personal");
                    break;
                case "clientlist":
                    define("MODULE", "clients/clientlist");
                    break;
                default:
                    define("MODULE", "error");
                    break;
            }
            break;

        case "stat":
            switch ($deep2) {
                case "clients":
                    define("MODULE", "stat/stat_clients");
                    break;
           
                    break;
                default:
                    define("MODULE", "error");
                    break;
            }
            break;


        case "dialogs": // Модальные окна
            switch ($deep2) {
                case "ticket_add":
                    define("MODULE", $deep1 . "/ticket_add");
                    break;
                

                default:
                    define("MODULE", "error");
                    break;
            }
            break;

        case "admin": //АДМИНПАНЕЛЬ
            if (
                /* strpos($_SERVER["REMOTE_ADDR"],"10.200.8.")===0 || */
                !empty($auth->auth["admin"]) ||
                $auth->auth["login"] == "sysadmin" ||
                1 == 1
            ) {
                switch ($deep2) {
                    case "doc_params":
                        define("MODULE", $deep1 . "/doc_params");
                        break;
                    case "hotels":
                        define("MODULE", $deep1 . "/hotels");
                        break;
                    case "directories":
                        define("MODULE", $deep1 . "/directories");
                        break; //поиск клиента
                    case "mails":
                        define("MODULE", $deep1 . "/mails");
                        break;
                    case "info":
                        define("MODULE", $deep1 . "/info");
                        break;
                    case "pay_confirm":
                        define("MODULE", $deep1 . "/pay_confirm");
                        break;
                    case "offices":
                        define("MODULE", $deep1 . "/offices");
                        break;
                    case "operators":
                        define("MODULE", $deep1 . "/operators");
                        break;
                    case "users":
                        define("MODULE", $deep1 . "/users");
                        break;
                    case "user_edit":
                        define("MODULE", $deep1 . "/user_edit");
                        break;
                    case "tasks":
                        define("MODULE", $deep1 . "/tasks");
                        break;

                    case "menu":
                        define("MODULE", $deep1 . "/menu");
                        break;
                    case "groups":
                        define("MODULE", $deep1 . "/groups");
                        break;
                    case "settings":
                        define("MODULE", $deep1 . "/settings");
                        break;
                    case "attr":
                        define("MODULE", $deep1 . "/attrributes");
                        break;
                    case "feedback":
                        define("MODULE", $deep1 . "/feedback");
                        break;
                    case "referal":
                        define("MODULE", $deep1 . "/referal");
                        break;
                    case "salary":
                        define("MODULE", $deep1 . "/salary");
                        break;
                    case "money_transfer":
                        define("MODULE", $deep1 . "/money_transfer");
                        break;
                    default:
                        define("MODULE", "error");
                        break;
                }
            } else {
                $deep2 = "403";
                define("MODULE", "error");
            }
            break;

        case "error": //ашипка
            define("MODULE", "error");
            break;
    }
} else {
    define("MODULE", "login");
     $p->set_var(array(
          "ERROR"             => $err,
          "FRM_LOGIN_ACTION"  => MAIN_URL,
          "LOGIN"             => $cook["login"],
          "CHALLENGE"         => $auth->sys["challenge"],
          "DOMAIN"            => $db_name,
          "USER_TYPE"         => $user_type,
      ));
    //break;
}



$MENU_LINE = "";

if (is_file(HTML_PATH . MODULE . ".ht")) {
    $p->set_file(MODULE, MODULE . ".ht");
} else {
    die("Файл шаблона " . MODULE . " не найден");
}


//Подключаем модуль
if (is_file(INC_PATH . MODULE . ".inc")) {
    require_once(INC_PATH . MODULE . ".inc");
} else {
    die("Программный модуль не обнаружен.");
}

require_once(INC_PATH . "top.inc");
require_once(INC_PATH . "left.inc");


unset($cook);
//Дисконнект базы
//unset($db,$db1,$dbo,$dbo1);
if (isset($auth->auth["uid"]))
    $uid = $auth->auth["uid"];
else
    $uid = 0;
//Глобальные переменные в шаблон
$p->set_var(array(
                "MAIN_URL" => MAIN_URL,
                "URI_URL" => URI_URL,
                "URI" => URI,
                "IMG_URL" => IMG_URL,
                "CSS_URL" => CSS_URL,
                "CSS_URL_MAIN" => CSS_URL_MAIN,
                "CSS_URL_EXTJS" => CSS_URL_EXTJS,
                "JQUERY_URL" => JQUERY_URL,
                "JQ_URL_JQUERY" => JQ_URL_JQUERY,
                "JS_URL" => JS_URL,
                "JS_URL_MAIN" => JS_URL_MAIN,
                "FILES_PATH" => FILES_PATH,

                "YEARS" => date("Y"),
                "MENU_LINE" => "Тур+" . $MENU_LINE,
                //"MENU_TREE"		=> $menu_tree,
                "MAIN_TITLE_IMG" => "",
                "MAIN_TITLE" => $TITLE,
                "TITLE" => "Тур+" . $TITLE,
                "DATE_TIME" => $WEEKDAY[date("w")] . ", " . show_date(date("Y-m-d"), 1),
                "TIME" => date("H:i"),
                "AUTH_ID" => $uid,
                "TODAY" => date("Y-m-d"),

            ));


//Парсим глобальные ошибки и статус
if (is_array($ERROR_GLOBAL) && !empty($ERROR_GLOBAL)) {
    $p->set_block("error", "error_global");
    $p->set_block("error_global", "error_global_list", "error_global_l");
    foreach ($ERROR_GLOBAL as $value) {
        $p->set_var("ERROR_GLOBAL", $value);
        $p->parse("error_global_l", "error_global_list", true);
    }
} elseif (is_array($STATUS_GLOBAL) && !empty($STATUS_GLOBAL)) {
    $p->set_block("status", "status_global");
    $p->set_block("status_global", "status_global_list", "status_global_l");
    foreach ($STATUS_GLOBAL as $value) {
        $p->set_var("STATUS_GLOBAL", $value);
        $p->parse("status_global_l", "status_global_list", true);
    }
}
if (is_array($ERROR_PHP) && !empty($ERROR_PHP) && IS_ADMIN) {
    $p->set_block("head", "php_errors");
    $p->set_var("ERROR_PHP", preg_replace("/[\n\t\r]+/", " ", implode("", $ERROR_PHP)));
}

//ЗАПИСЬ ПОСЕЩЕННОЙ СТРАНИЦЫ
/*
  if(!empty($_SESSION["incoming_call"]) && !empty($_SESSION["call_id"]) && !empty($TITLE)) {
  $dbo->query("INSERT INTO sa_pages_stat"]." (call_id, name) VALUES (".$_SESSION["call_id"].", '".$TITLE."')");
  }
 */


//время загрузки страницы
$STAT_LOADING = array("uri" => URI, "time_php" => round(microtime(1) - $TIME_PHP_HEAD - $TIME_SQL, 4), "time_sql" => $TIME_SQL, "time" => microtime(1));
$p->set_var("STAT_LOADING", json_encode($STAT_LOADING));

$p->set_var("LOAD_TIME", "&nbsp;&nbsp;&nbsp;php=" . round(microtime(1) - $TIME_PHP_HEAD - $TIME_SQL, 4) . ", sql=" . $TIME_SQL . ", dbUser=" . $dbo->User);
/* if(IS_ADMIN) {
  $p->set_var("LOAD_TIME", "&nbsp;&nbsp;&nbsp;php=".round(microtime(1)-$TIME_PHP_HEAD-$TIME_SQL,4).", sql=".$TIME_SQL.", dbUser=".$dbo->User);
  } else {
  $p->set_var("LOAD_TIME", "&nbsp;&nbsp;&nbsp;Время загрузки страницы=".round(microtime(1)-$TIME_PHP_HEAD,4)." сек.");
  } */


//Вывод html
if (isset($PRINT)) {
    $p->parse_all(array("error", "head", "head_print", "foot", "foot_print", "main_top", "main_bottom", MODULE));
} else {
    $p->parse_all(array("error", "head", "foot", "main_top", "main_bottom", MODULE));


}
$p->p(MODULE);

/*
$hostName = "";
$userName = "root";
$password = "";
$databaseName = "tur";
if (!($link=mysql_connect($hostName,$userName,$password)))              { echo $hostName;
    printf("Ошибка при соединении с MySQL !\n");
    exit();
}
if (!mysql_select_db($databaseName, $link)) {
    printf("Ошибка базы данных !");
    exit();
}*/

//if(!isset($PRINT)) echo '<center><font style="font-size:7pt; color:silver;">'.round(microtime(1)-$HeadTime,4).'</font></center>';
?>