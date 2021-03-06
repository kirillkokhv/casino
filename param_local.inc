<?php
### Описание базы данных ###
setlocale(LC_CTYPE, 'ru_RU.CP1251', 'rus_RUS.CP1251', 'Russian_Russia.1251');

class DB_Info implements ArrayAccess {
	public $auth_sessions         = "auth_sessions";
	public $auth_exp              = "auth_exp"; //дата последнего визита пользователя
	public $auth_menu             = "auth_menu";
	public $auth_menu2objects     = "auth_menu2objects";
	public $auth_users            = "auth_users";
	public $auth_users_inf        = "auth_users_inf";
	public $auth_groups           = "auth_groups";
	public $auth_service          = "auth_services";
	public $auth_access           = "auth_access";
	public $auth_atributs         = "auth_atributs";
	public $auth_atributs_help    = "auth_atributs_help";
	public $auth_values           = "auth_values";
	public $auth_objects_atributs = "auth_objects2atributs";
	public $auth_tmp_access       = "access.tmp_access";
	public $auth_tmp_access_v     = "access.tmp_access_v";
	public $auth_sp_slovar        = "sp_slovar";    //словарь существительных
	public $auth_ssz_users        = "crm.auth@ssz";
	public $auth_kredit_users     = "kredit.auth@kredit";
		
	function offsetGet($name) {
		return $this->$name;
	}
	function offsetExists($name) {
		if(isset($this->$name))
	  		return true;
		else
			return false;
	}
	function offsetSet($name,$val) {
	  return false;
	}
	function offsetUnset($name) {
	  return false;
	}
}

$DB = new DB_Info;

$DB_name = explode(".",$_SERVER["SERVER_NAME"]);
$crm_account = $DB_name = $DB_name[0]=="tur1"?"tur":$DB_name[0];
//$mdb = "p329332_tourcrm";
$mdb="tur";


if(!empty($_SERVER["SERVER_NAME"])) {
	define("GLOBAL_PATH", preg_replace("/\/[^\/]+$/","",$_SERVER["DOCUMENT_ROOT"])."/" ,0);
	define("MAIN_URL", ((empty($_SERVER["HTTPS"]) || $_SERVER["HTTPS"]!="on") ? "http://" : "https://").$_SERVER["SERVER_NAME"].((isset($_SERVER["SERVER_PORT"]) && $_SERVER["SERVER_PORT"]!=80 && $_SERVER["SERVER_PORT"]!=443) ? ":".$_SERVER["SERVER_PORT"] : "")."/"  ,0);
} else {
	//CLI
	define("GLOBAL_PATH", "/srv/" ,0);
	define("MAIN_URL", "" ,0);
}


if(
	(isset($_SERVER["SERVER_NAME"]) && ($_SERVER["SERVER_NAME"]=="tur1" ||$_SERVER["SERVER_NAME"]=="tur2"))
) {
	define("IS_ADMIN",  true, 0);
} else {
	define("IS_ADMIN",  false, 0);
}
if(stristr($_SERVER["SERVER_NAME"],"edinotur.ru"))  $www = "public_html"; 
//if($_SERVER["SERVER_NAME"]=="crm.edinotur.ru") $www = "public_html"; 
else $www="www";
define("LIBS",       GLOBAL_PATH.$www."/libs/",0);
define("MAIN_PATH",  GLOBAL_PATH.$www."/",0);
define("HTML_PATH",  MAIN_PATH."html/",0);
define("INC_PATH",   MAIN_PATH."include/",0);

define("LOGS_PATH",  GLOBAL_PATH."logs/",0);
define("LOGS_AUTH",  LOGS_PATH."auth.log",0);
define("LOGS_LIBS",  LOGS_PATH."libs.log",0);

define("IMG_URL",      MAIN_URL."img/",0);
define("CSS_URL",      MAIN_URL."css/",0);
define("CSS_URL_MAIN", MAIN_URL."css/css.20121030.css.gz",0);
define("CSS_URL_EXTJS", MAIN_URL."js/extjs4/resources/css/ext-all-neptune.css",0);
define("JS_URL",       MAIN_URL."js/",0);
define("JQUERY_URL", MAIN_URL."js/jquery/",0);

define("JQ_URL_JQUERY",  MAIN_URL."js/jquery/jquery.js",0);
define("JS_URL_MAIN",  MAIN_URL."js/main.js",0);

define("FILES_PATH", GLOBAL_PATH."files/",0);
define("FILES_TMP_PATH", GLOBAL_PATH."files_tmp/",0);
define("LEN",   10,0);
define("TIMEOUT",   36000);

#####################
$MOUNTHS=array("-","Январь","Февраль","Март","Апрель","Май","Июнь","Июль","Август","Сентябрь","Октябрь","Ноябрь","Декабрь");
$RMOUNTH=array("-","Января","Февраля","Марта","Апреля","Мая","Июня","Июля","Августа","Сентября","Октября","Ноября","Декабря");
$WEEKDAY=array("Воскресенье","Понедельник","Вторник","Среда","Четверг","Пятница","Суббота","Воскресенье");
#####################


### Пареметры системы ###
define("MAIL_ADMIN",          "crm_admin11@edinotur.ru",0); # e-mail разработчиков
define("FILE_SERVER",         "77.222.42.115",0);  # FTP сервер для хранения файлов
define("FILE_SERVER_LOGIN",   "edinotouru_crm",0);
define("FILE_SERVER_PASS",    "Zx14cv47b",0);
define("FILE_SERVER_DIR",     "/files/",0);
define("DB_PREFIX", "p329332");

$ObjType=array(
	"auth"     => 1,
	"groups"   => 2
	
);

$MenuGroups=array(
    1 => "Основные",
    2 => "Статистика",
    3 => "Администрирование"
    

);




$FILE_TYPES=array(
"application/msword"            => array("compress" => 5, "ico" => "word.ico",    "ext" => ".doc"),
"application/rtf"               => array("compress" => 5, "ico" => "word.ico",    "ext" => ".rtf"),
"text/rtf"                      => array("compress" => 5, "ico" => "word.ico",    "ext" => ".rtf"),
"text/csv"                      => array("compress" => 5, "ico" => "document-excel-csv.png",    "ext" => ".csv"),
"application/vnd.ms-excel"      => array("compress" => 5, "ico" => "excel.ico",   "ext" => ".xls"),
"application/vnd.ms-powerpoint" => array("compress" => 1, "ico" => "power.ico",   "ext" => ".ppt"),
"application/pdf"               => array("compress" => 1, "ico" => "document-pdf.png", "ext" => ".pdf"),
"application/zip"               => array("compress" => 0, "ico" => "arj.gif",     "ext" => ".zip"),
"application/x-zip"             => array("compress" => 0, "ico" => "arj.gif",     "ext" => ".zip"),
"application/x-zip-compressed"  => array("compress" => 0, "ico" => "arj.gif",     "ext" => ".zip"),
"application/x-rar"             => array("compress" => 0, "ico" => "arj.gif",     "ext" => ".rar"),
"application/x-arj"             => array("compress" => 0, "ico" => "arj.gif",     "ext" => ".arj"),
"image/png"                     => array("compress" => 0, "ico" => "image.ico",   "ext" => ".png"),
"image/gif"                     => array("compress" => 0, "ico" => "image.ico",   "ext" => ".gif"),
"image/jpeg"                    => array("compress" => 0, "ico" => "image.ico",   "ext" => ".jpg"),
"image/pjpeg"                   => array("compress" => 0, "ico" => "image.ico",   "ext" => ".jpg"),
"image/tiff"                    => array("compress" => 1, "ico" => "image.ico",   "ext" => ".tif"),
"image/vnd.ms-modi"             => array("compress" => 1, "ico" => "image.ico",   "ext" => ".mdi"),
"image/bmp"                     => array("compress" => 1, "ico" => "image.ico",   "ext" => ".bmp"),
"application/vnd.oasis.opendocument.graphics"     => array("compress" => 3, "ico" => "document-image.png", "ext" => ".odg"),
"application/vnd.oasis.opendocument.presentation" => array("compress" => 3, "ico" => "power.ico",          "ext" => ".odp"),
"application/vnd.oasis.opendocument.spreadsheet"  => array("compress" => 5, "ico" => "excel.ico",          "ext" => ".ods"),
"application/vnd.oasis.opendocument.text"         => array("compress" => 5, "ico" => "word.ico",           "ext" => ".odt")
);

?>
