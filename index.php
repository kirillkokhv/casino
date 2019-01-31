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



if (!isset($db)) {
    $db = new DB_MySQL(array("ShowErr" => IS_ADMIN));
} else {
    $db->clear();
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
 

    //}
} else {
    $auth->auth["fio"] = "нет авторизации";
    $deep1 = "index";
}
#######
##### РАЗДЕЛЫ САЙТА



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

        case "monitoring":  //закрываем страницу
            define("MODULE", "pages/monitoring");
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
if ($auth->CheckAuth()) {
   
    $new_mails = 0;
    $active_tasks = 0;

    $p->set_var(array(
                    "USER_FIO" => $auth->auth["fio"],
                    "USER_AVATAR" => $auth->auth["avatar"],
                    "NEW_MAILS" => $new_mails,
                    "ACTIVE_TASKS" => $active_tasks,

                ));

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