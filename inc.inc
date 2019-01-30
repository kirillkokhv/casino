<?php

if (stristr($_SERVER["SERVER_NAME"], "tur2")) require("param_local.inc");
else require("param.inc");
set_vars(array("deep1", "deep2", "deep3", "deep4", "detect", "user_type"));
set_vars(array("page"), 1);
if ($deep1 == "") {
    $deep1 = "index";
}

function __autoload($class)
{
    if ($class == "Session")
        include(LIBS . "session.inc");
    elseif ($class == "Cookie") 
        include(LIBS . "cookie.inc");
    elseif ($class == "Auth") 
        include(LIBS . "auth.inc");
    elseif ($class == "Template") 
        include(LIBS . "template.inc"); 
    elseif ($class == "STR")
        include(LIBS . "str_func.inc");
    elseif ($class == "Fonetic")
        include(LIBS . "fonetic.inc");
    elseif ($class == "Log")
        include(LIBS . "logs.inc");
    elseif (strpos($class, "DB_MySQL") === 0) {
        include(LIBS . "db_mysql.inc");
        
    } 
   
//elseif($class=="Mail")                include("Mail.php");
    //elseif($class=="Mail_mime")           include("Mail/mime.php");

    elseif ($class == "Excel")
        include(LIBS . "excel.inc");
   
    //elseif($class=="PHPWord")            include(LIBS . "phpword.inc");

    elseif (strpos($class, "PHPExcel") === 0) {
        //require_once(LIBS."PHPExcel/PHPExcel.php");
        //require_once(LIBS."PHPExcel/PHPExcel/IOFactory.php");
        if (defined("PHPEXCEL_ROOT")) {
            include(PHPEXCEL_ROOT . str_replace("_", DIRECTORY_SEPARATOR, $class) . ".php");
        }
    } else {
        die("Попытка подключить неизвестный класс: " . $class);
    }
}

#### ФУНКЦИИ ####
// определяемая пользователем функция обработки ошибок

function my_error_handler($errno, $errmsg, $filename, $linenum, $vars)
{
    global $ERROR_PHP;
    if (empty($ERROR_PHP))
        $ERROR_PHP = array();
    // определяем ассоциативный массив строки ошибки
    // в действительности единственные входы, которые
    // мы должны рассмотреть - это E_WARNING, E_NOTICE, E_USER_ERROR,
    // E_USER_WARNING и E_USER_NOTICE
    $errortype = array(
        E_ERROR           => "Error",
        E_WARNING         => "Warning",
        E_PARSE           => "Parsing Error",
        E_NOTICE          => "Notice",
        E_CORE_ERROR      => "Core Error",
        E_CORE_WARNING    => "Core Warning",
        E_COMPILE_ERROR   => "Compile Error",
        E_COMPILE_WARNING => "Compile Warning",
        E_USER_ERROR      => "User Error",
        E_USER_WARNING    => "User Warning",
        E_USER_NOTICE     => "User Notice",
        E_STRICT          => "Runtime Notice",
    );
    // набор ошибок, на которые переменный след будет сохранен
    $user_errors = array(E_USER_ERROR, E_USER_WARNING, E_USER_NOTICE);
    if (MAIN_URL && IS_ADMIN) {
        $ERROR_PHP[] = '<div style="white-space: nowrap;"><b>' . $errortype[$errno] . '</b> ' . preg_replace("/[\\\]/", "/", $filename) . ' [' . checkstr($linenum) . '] ' . checkstr($errmsg) . '</div>';
    }
    if (in_array($errno, array(E_ERROR, E_PARSE, E_USER_ERROR, E_USER_WARNING, E_USER_NOTICE))) {
        Log::get("error", $errortype[$errno], $errmsg, $filename, "line=" . $linenum);
    }
}

set_error_handler("my_error_handler");

# редирект на ту же страницу, но с отметкой, что операция уже выполнена. необходима для блокировки повторной отправки

function redirect($mess, $url = "", $err = 0)
{
    global $STATUS_GLOBAL, $ERROR_GLOBAL, $sess;
    if ($err == 0) {
        if ($mess != "")
            $STATUS_GLOBAL[] = $mess;
        $_SESSION["is_send_status_mes"] = $STATUS_GLOBAL;
    } else {
        if ($mess != "")
            $ERROR_GLOBAL[] = $mess;
        $_SESSION["is_send_error_mes"] = $ERROR_GLOBAL;
    }
    if ($url == "") {
        header("Location: " . URI_URL);
    } else {
        header("Location: " . $url);
    }
    exit;
}

# установка атрибут пользователя

function set_user_atributs()
{
    global $DB, $dbo, $auth, $ObjType;
    if (isset($auth->auth["user_groups"][$auth->auth["user_group_changed"]]))
        $group_id = $auth->auth["user_groups"][$auth->auth["user_group_changed"]]["group_id"];
    $sqlstr = "SELECT a.name code, v.text value
		    FROM " . $DB["auth_objects_atributs"] . " a2v," . $DB["auth_atributs"] . " a," . $DB["auth_values"] . " v WHERE
		    a2v.atribut_id=a.atribut_id AND a2v.value_id=v.value_id
		    AND (
		    (a2v.object_id=" . $auth->auth["user_id"] . " AND a2v.object_type=" . $ObjType["auth"] . ") OR
		    (a2v.object_id=" . $group_id . " AND a2v.object_type=" . $ObjType["groups"] . ")
		    )
		    AND a.name!='access'
		    ORDER BY a2v.object_type DESC
		    ";
    $dbo->query($sqlstr);
    if ($dbo->check_rows()) {
        $auth->auth["attr"] = array();
        while ($dbo->next_record()) {
            /*
              if( isset($auth->auth["attr"][$dbo["code"]]) && !is_array($auth->auth["attr"][$dbo["code"]]) ) {
              $tmp=$auth->auth["attr"][$dbo["code"]];
              $auth->auth["attr"][$dbo["code"]]=array();
              $auth->auth["attr"][$dbo["code"]][]=$tmp;
              $auth->auth["attr"][$dbo["code"]][]=$dbo["value"];
              } else {
              if( isset($auth->auth["attr"][$dbo["code"]]) && is_array($auth->auth["attr"][$dbo["code"]]) ) {
              $auth->auth["attr"][$dbo["code"]][]=$dbo["value"];
              } else {
              $auth->auth["attr"][$dbo["code"]]=$dbo["value"];
              }
              }
             */
            $auth->auth["attr"][$dbo["code"]] = $dbo["value"];
        }
    }
}

### Перевод даты в цифровой формат

function datedigit($date = 0, $f = 0)
{
    if ($date == 0) {
        $date = date("d.m.Y");
    }
    if (preg_match("/-/", $date)) {
        $date = show_date($date);
    }
    if (preg_match("/ /", $date)) {
        list($date, $time) = explode(" ", $date);
    } else {
        $time = "";
    }
    if (empty($time))
        $time = "00:00:00";
    list($d, $m, $y) = explode(".", $date);
    $d = intval($d);
    $m = intval($m);
    $y = intval($y);
    list($hh, $mm, $ss) = explode(":", $time);
    $hh = intval($hh);
    $mm = intval($mm);
    $ss = intval($ss);
    if ($y < 100) {
        if ($y > 50)
            $y += 1900;
        else
            $y += 2000;
    }
    if ($f)
        return mktime($hh, $mm, $ss, $m, $d, $y);
    else
        return $y * 32140800 + $m * 2678400 + $d * 86400 + $hh * 3600 + $mm * 60 + $ss;
}

### Обработка строки

function checkstr($str, $len = 0, $st = 0)
{
    if ($len > 0) {
        $str = substr($str, 0, $len);
    }
    if ($st == 0) {
        global $ERROR_PHP;
        $str = htmlspecialchars($str, ENT_QUOTES);
    }
    //$str=preg_replace("/[\\][\"']/","&quot;",$str);
    $str = preg_replace("/[\"']/", "&quot;", $str);
    $str = preg_replace("/[\\']/", "", $str);

    return $str;
}

### Проверка даты 01.01.2009

function currdate($str)
{
    if (strlen($str) >= 8) {
        $data = explode(".", $str);
        if (
            count($data) == 3 && strlen($data[0]) <= 2 && strlen($data[1]) <= 2 && strlen($data[2]) <= 4 && checkdate((int)$data[1], (int)$data[0], (int)$data[2])
        ) {
            return true;
        } else {
            return false;
        }
    }

    return false;
}

### Проверка времени 11:00[:00]

function currtime($str, $sec = 0)
{
    if (preg_match("/:/", $str)) {
        $data = explode(":", $str);
        if (empty($data[0]) || $data[0] < 0 || $data[0] > 23)
            return false;
        if (empty($data[1]) || $data[1] < 0 || $data[1] > 59)
            return false;
        if ($sec && (empty($data[1]) || $data[1] < 0 || $data[1] > 59))
            return false;

        return true;
    }

    return false;
}

### Проверка периода
# period - в днях

function checkperiod($start, $end, $period)
{
    $start  = datedigit($start, 1);
    $end    = datedigit($end, 1);
    $period = $period * 24 * 60 * 60 + 1;
    if ($end - $start > $period) {
        return false;
    } else {
        return true;
    }
}

function show_date($date = "", $st = 0)
{
    global $Mounth_Array, $rMounth_Array;
    if ($date != "") {

        if (preg_match("/ /", $date)) {
            list($date, $time) = explode(" ", $date);
        } else {
            $time = "";
        }
        $ret = "";

        if (preg_match("/^[0-9]{4}-/", $date)) {
            list($y, $m, $d) = explode("-", $date);
            if ($st <= 0) {
                if (preg_match("/^[0-9]{1,2}/", $d)) {
                    $ret = sprintf("%02d", $d) . ".";
                }
                $ret .= sprintf("%02d", $m) . "." . $y;
            }
        } else {
            $dd = explode("\.", $date);
            if (count($dd) == 3) {
                $y = $dd[2];
                $m = $dd[1];
                $d = $dd[0];
            } else {
                $y = $dd[1];
                $m = $dd[0];
            }
            if ($st <= 0) {
                $ret = $y . "-" . sprintf("%02d", $m);
                if (preg_match("/^[0-9]{1,2}/", $d)) {
                    $ret .= "-" . sprintf("%02d", $d);
                }
            }
        }

        if ($st > 0) {
            if (preg_match("/^[0-9]{1,2}$/", $d)) {
                $ret = sprintf("%02d", $d) . " " . $rMounth_Array[($m * 1)];
            } else {
                $ret = $Mounth_Array[($m * 1)];
            }
            $ret .= " " . $y;
        }

        if (preg_match("/^[0-9]{1,2}:/", $time)) {
            $ret .= " " . $time;
        }
    }

    return $ret;
}

//Форматируем продолжительность
function show_duration($duration, $format = "s")
{
    $h = $m = $s = 0;
    if (strpos($duration, "-") !== false) {
        $sign = "-";
    } else {
        $sign = "";
    }
    $duration = abs($duration);
    if ($format == "s") { //из секунд
        $h = floor($duration / 3600);
        $m = floor(($duration - $h * 3600) / 60);
        $s = floor(($duration - $h * 3600) - $m * 60);
    }
    if ($h < 10)
        $h = "0" . $h;
    if ($m < 10)
        $m = "0" . $m;
    if ($s < 10)
        $s = "0" . $s;

    return $sign . $h . ":" . $m . ":" . $s;
}

### Перобразование русских символов в hex

function code_get($str = '')
{
    global $auth;
    $ret = "";
    for ($i = 0; $i <= strlen($str) - 1; $i++) {
        $str = preg_replace("/\//", "#", $str);
        $ord = ord(substr($str, $i, 1));
        if ($ord <= 32) {
            $ord = 35;
        }
        $ret .= "%" . dechex($ord);
    }

    return $ret;
}

# отображение атрибутов

function show_atr($cur = 0, $st = 0)
{
    global $dbo, $DB;
    $sqlstr = "SELECT atribut_id, description FROM " . $DB["auth_atributs"];
    if ($st > 0) {
        $sqlstr .= " WHERE atribut_id=" . $cur;
    }
    //else{$sqlstr.=" WHERE a_name!='access'";}
    $sqlstr .= " ORDER BY description";
    $dbo->query($sqlstr);
    $ret = '';
    if ($dbo->check_rows()) {
        if ($st == 0) {
            while ($dbo->next_record()) {
                $ret .= '<option value="' . $dbo["atribut_id"] . '"';
                if ($cur == $dbo["atribut_id"]) {
                    $ret .= ' selected';
                }
                $ret .= '>' . STR::big_to_small($dbo["description"]) . '</option>';
            }
        } else {
            $dbo->next_record();
            $ret = STR::big_to_small($dbo["description"]);
        }
    }

    return $ret;
}

# отображение групп

function show_grp($cur = 0)
{
    global $dbo, $dbo1, $DB;

    $ret = '';

    $ret .= '<optgroup label="' . $dbo["name"] . '">';
    $dbo1->query("SELECT group_id, name FROM " . $DB["auth_groups"] . " WHERE locked=0 ORDER BY name");
    while ($dbo1->next_record()) {
        $ret .= '<option value="' . $dbo1["group_id"] . '"';
        if ($cur == $dbo1["group_id"]) {
            $ret .= ' selected';
        }
        $ret .= '>' . $dbo1["name"] . '</option>';
    }
    $ret .= '</optgroup>';

    return $ret;
}

### Проверка картинка

function loadfoto($file, $new_name)
{
    $max_image_width  = 3000;
    $max_image_height = 3000;
    $max_image_size   = 2 * 1024 * 1024;
    $valid_types      = "jpg";

    $err = false;
    if (isset($file)) {
        if (is_uploaded_file($file['tmp_name'])) {
            $filename = $file['tmp_name'];
            $ext      = substr($file['name'], 1 + strrpos($file['name'], "."));
            $ext      = strtolower($ext);
            if ($ext == "jpeg") {
                $ext = "jpg";
            }
            $new_name = MAP_PATH . $new_name . "." . $ext; //новое имя и адрес фотки
            @unlink($new_name); //удаляем, если такой уже есть
            if (filesize($filename) > $max_image_size) {
                $err = "Размер изображения более 2Мб";
            } elseif ($ext != $valid_types) {
                $err = "Некорректный тип файла";
            } else {
                $size = GetImageSize($filename);
                if (($size) && ($size[0] <= $max_image_width) && ($size[1] <= $max_image_height)) {
                    if (@move_uploaded_file($filename, $new_name)) {
                        chmod($new_name, 0644);
                        $err = "uploaded";
                    } else {
                        $err = "Изображение не перемещено на сайт";
                    }
                } else {
                    $err = "Недопустимые размеры изображения";
                }
            }
        } else {
            $err = "Файл не существует";
        }
    } else {
        $err = "Файл не существует.";
    }

    return $err;
}

### Загрузка документа

function loaddoc($file, $new_name)
{
    $max_image_size = 1 * 1024 * 1024;
    $valid_types    = array("doc", "xls");

    $err = $ext = false;
    if (isset($file)) {
        if (is_uploaded_file($file['tmp_name'])) {
            $filename = $file['tmp_name'];
            $ext      = substr($file['name'], 1 + strrpos($file['name'], "."));
            $ext      = strtolower($ext);
            $new_name = DOCS_PATH . $new_name . "." . $ext; //новое имя и адрес фотки
            @unlink($new_name); //удаляем, если такой уже есть
            if (filesize($filename) > $max_image_size) {
                $err = "Размер файла более 1Мб";
            } elseif (!in_array($ext, $valid_types)) {
                $err = "Некорректный тип файла";
            } else {
                if (@move_uploaded_file($filename, $new_name)) {
                    chmod($new_name, 0644);
                    $err = "uploaded";
                } else {
                    $err = "Документ не перемещен на сайт";
                }
            }
        } else {
            $err = "Файл не существует";
        }
    } else {
        $err = "Файл не существует.";
    }

    return array($err, $ext);
}

### Загрузка документа
// $cursor - указатель массива $file["name"][$cursor]

function loadfile($file, $new_name, $cursor = null)
{
    global $FILE_TYPES;
    if ($cursor === null) {
        $file_name     = $file["name"];
        $file_tmp_name = $file["tmp_name"];
        $file_size     = $file["size"];
    } else {
        $file_name     = $file["name"][$cursor];
        $file_tmp_name = $file["tmp_name"][$cursor];
        $file_size     = $file["size"][$cursor];
    }

    $new_dir_mm = date("Y-m") . "/"; //директория для файла (по месяцу)
    if (!is_dir(FILES_TMP_PATH . $new_dir_mm)) {
        mkdir(FILES_TMP_PATH . $new_dir_mm); //создаем директорию для файла
        //chmod(FILES_TMP_PATH.$new_dir_mm, 0775); //устанавливаем права доступа на директорию
    }

    $new_dir = date("Y-m") . "/" . date("d") . "/"; //директория для файла (по дню)

    if (!is_dir(FILES_TMP_PATH . $new_dir)) {
        mkdir(FILES_TMP_PATH . $new_dir); //создаем директорию для файла
        //chmod(FILES_TMP_PATH.$new_dir, 0775); //устанавливаем права доступа на директорию
    }

    $new_name = $new_name . ".rar"; //название файла, которое в итоге останется
    //$new_name = "21.rar";
    $err = "Не известная ошибка";

    $mime = $old_name = "";
    if ($file_tmp_name != "") {

        if (is_uploaded_file($file_tmp_name)) { //проверяем,что файл пользователь загрузил на сервер
            //$ext = substr($file['name'], 1 + strrpos($file['name'], "."));
            $mime = get_mime($file_tmp_name, $file_name);
            //	if(isset($FILE_TYPES[$mime])) {
            $size = filesize($file_tmp_name);
            //if ($size < FILE_SIZE_MAX) { //проверяем, что размер файла допустимый
            if (file_exists(FILES_TMP_PATH . $file_name))
                unlink(FILES_TMP_PATH . $file_name); //если каким-то образом уже такой файл был загружен, то удаляем
            if (move_uploaded_file($file_tmp_name, FILES_TMP_PATH . $file_name)) { //копируем из временной директории в директорию с файлами
                if (is_file(FILES_TMP_PATH . $file_name) && $file_size == $size) { //проверяем, что скопировалось
                    //Архивируем
                    //chown(FILES_PATH.$file_name, "www");
                    //chgrp(FILES_PATH.$file_name, "max");
                    // $x='/usr/local/bin/rar a -m'.$FILE_TYPES[$mime]["compress"].' '.FILES_TMP_PATH.$new_dir.$new_name.' "'.FILES_TMP_PATH.$file_name.'"'; //архивируем
                    // $x=`$x`;
                    unlink(FILES_TMP_PATH . $file_name); //удаляем не архивный файл
                    chmod(FILES_TMP_PATH . $new_dir . $new_name, 0664); //устанавливаем права доступа на архив
                    //if(is_file(FILES_TMP_PATH.$new_dir.$new_name)) { //проверяем, что архив есть
                    /* //Копируем на FTP

                      if($ftp = ftp_connect(FILE_SERVER)) { // соединение с сервером
                      if(ftp_login($ftp, FILE_SERVER_LOGIN, FILE_SERVER_PASS)) { // вход на FTP сервер
                      if(!ftp_chdir($ftp, FILE_SERVER_DIR.$new_dir_mm)){
                      ftp_mkdir($ftp, FILE_SERVER_DIR.$new_dir_mm); // создаёт директорию
                      }
                      if(!ftp_chdir($ftp, FILE_SERVER_DIR.$new_dir)){
                      ftp_mkdir($ftp, FILE_SERVER_DIR.$new_dir); // создаёт директорию
                      ftp_chdir($ftp, FILE_SERVER_DIR.$new_dir); // устанавливаем директорию в которой находимся
                      }
                      if(ftp_put($ftp, $new_name, FILES_TMP_PATH.$new_dir.$new_name, FTP_BINARY)) { // загружает файл на FTP сервер
                      //ftp_chmod($ftp, 0644, $new_name); // устанавливает права доступа к файлу
                      unlink(FILES_TMP_PATH.$new_dir.$new_name);
                      $err = "";
                      } else {
                      Log::get("error", "FTP", "UPLOAD FILE ".FILES_TMP_PATH.$new_dir.$new_name);
                      $err = "Ошибка загрузки файла на FTP";
                      }
                      } else {
                      Log::get("error", "FTP", "LOGIN ".FILE_SERVER."@".FILE_SERVER_LOGIN);
                      $err = "Ошибка при подключении к FTP";
                      }
                      ftp_close($ftp); // закрываем соединение с FTP сервером
                      } else {
                      Log::get("error", "FTP", "CONECT ".FILE_SERVER);
                      $err = "Ошибка при подключении к FTP";
                      } */

                    $err = "";
                    //	} else {
                    //	Log::get("error", "ARJ", $x);
                    //	$err = "Ошибка при архивировании файла";
                    //}
                } else {
                    $err = "Ошибка при сохранении файла";
                }
            } else {
                $err = "Документ не перемещен в систему";
            }
            //} else {
            //	$err = "Размер файла не должен превышать ".round(FILE_SIZE_MAX/1024/1024,2)."Мб";
            //}
            //	} else {
            //	$err = "Тип файлов ".$mime." недопустим";
            //	}
        } else {
            $err = "Файл не загружен";
        }
    } else {
        $err = "Файл не существует";
    }
    if ($err == "" && $mime != "") {
        return array(
            "status"   => true,
            "mime"     => addslashes($mime),
            "old_name" => STR::truncate(preg_replace("/\..+$/", "", checkstr($file_name)), 128),
            "new_file" => FILES_TMP_PATH . $new_dir . $new_name,
            "size"     => $size,
        );
    } else {
        return array("status" => false, "error" => $err);
    }
}

### Скачивание файла

function getfile($file_id, $file_path)
{
    $unpack_file = false;
    $pack_file   = FILES_TMP_PATH . $file_path . "/" . $file_id . ".rar";
    $err         = "Не известная ошибка";
    //print_r($file_id);exit;
    if ($file_path && $file_id) {
        //Скачиваем


        if (!is_file($pack_file)) {
            if (!file_exists(FILES_TMP_PATH . $file_path . "/")) {
                mkdir(FILES_TMP_PATH . $file_path, 0777);
            }
            if ($ftp = ftp_connect(FILE_SERVER)) {
                if (ftp_login($ftp, FILE_SERVER_LOGIN, FILE_SERVER_PASS)) {
                    /* if(ftp_mkdir($ftp, FILE_SERVER_DIR.$new_dir)) {
                      ftp_chmod($ftp, 0755, FILE_SERVER_DIR.$new_dir);
                      } */
                    ftp_chdir($ftp, FILE_SERVER_DIR . $file_path);
                    if (!ftp_get($ftp, $pack_file, FILE_SERVER_DIR . $file_path . "/" . $file_id . ".rar", FTP_BINARY)) {
                        Log::get("error", "FTP", "DOWNLOAD FILE " . $file_path . "/" . $file_id . ".rar");
                    }
                } else {
                    Log::get("error", "FTP", "LOGIN " . FILE_SERVER . "@" . FILE_SERVER_LOGIN);
                }
                ftp_close($ftp);
            } else {
                Log::get("error", "FTP", "CONECT " . FILE_SERVER);
            }
        }


        /* if(!is_file($pack_file)) {
          if(!file_exists(FILES_PATH.$file_path."/")) {
          mkdir(FILES_PATH.$file_path);
          chmod(FILES_PATH.$file_path, 0775);
          }
          if(is_file(ISCSI_PATH.$file_path."/".$file_id.".rar")) {
          rename(ISCSI_PATH.$file_path."/".$file_id.".rar", $pack_file);
          chmod($pack_file, 0664);
          } else {
          Log::get("error", "ISCSI", "FILE NOT EXISTS ".ISCSI_PATH.$file_path."/".$file_id.".rar");
          }
          } */
        if (is_file($pack_file)) {
            //Распаковываем
            $x           = '/usr/local/bin/rar p -inul ' . $pack_file;
            $unpack_file = `$x`;
            $err         = "";
            unlink($pack_file);
        } else {
            $err = "Файл не найден";
        }
    } else {
        $err = "Файл не существует";
    }

    if ($err == "" && $unpack_file != "") {
        return array("status" => true, "file" => $unpack_file);
    } else {
        return array("status" => false, "error" => $err);
    }
}

### Определяем тип файла

function get_mime($file, $name = "")
{
    $ext = strtolower(substr($name, 1 + strrpos($name, ".")));

    if ($ext === "xls" || $ext === "xlsx") {
        $mime = "application/vnd.ms-excel";
    } elseif ($ext === "doc" || $ext === "docx") {
        $mime = 'application/msword';
    } elseif ($ext === "csv") {
        $mime = "text/csv";
    } elseif ($ext === "odt") {
        $mime = "application/vnd.oasis.opendocument.text";
    } elseif ($ext === "ods") {
        $mime = "application/vnd.oasis.opendocument.spreadsheet";
    } elseif ($ext === "odp") {
        $mime = "application/vnd.oasis.opendocument.presentation";
    } elseif ($ext === "odg") {
        $mime = "application/vnd.oasis.opendocument.graphics";
    } elseif ($ext === "mdi") {
        $mime = "image/vnd.ms-modi";
    } else {
        $mime = '/usr/bin/file -ib "' . $file . '"';
        $mime = `$mime`;
        list($mime,) = explode(";", $mime);
        $mime = trim($mime);
    }

    return $mime;
}

//подсчёт страниц
function pages($sum, $len, $url = "", $anchor = "", $page)
{

    if (!$page) global $page;

    $max_show_pages = 4; //кол-во цифр на странице
    if (strstr($url, "?")) {
        $url .= "&amp;page=";
    } else {
        $url .= "?page=";
    }
    if ($anchor != "") {
        $anchor = "#" . $anchor;
    }
    $all_pages = ceil($sum / $len);
    $page++;
    if ($page < 1) {
        $page = 1;
    } elseif ($page > $all_pages) {
        $page = $all_pages;
    }

    $start = $page - floor($max_show_pages / 2);
    if ($start < 1) {
        $start = 1;
    }
    if ($start > ($all_pages - $max_show_pages)) {
        $start = $all_pages - $max_show_pages;
    }
    $finish = $page + floor($max_show_pages / 2);
    if ($finish > $all_pages) {
        $finish = $all_pages;
    } elseif ($finish < $max_show_pages && $max_show_pages < $all_pages) {
        $finish = $finish + ($max_show_pages - $finish);
    }
    $pages = "";
    for ($i = $start; $i < $finish + 1; $i++) {

        if ($i > 0) {
            $link_i = (($i - 1) * $len + 1) . "-" . (($i - 1) * $len + $len);
            if ($i == $page) {

                $pages .= "<span class=\"cur_page\">&nbsp;" . $i . "&nbsp;</span>";

            } else {
                $pages .= "&nbsp;<a class=\"pages num\" href=\"" . $url . ($i - 1) . $anchor . "\">" . $i . "</a>&nbsp;";
            }
        }
    }

    if ($pages == "") {
        $pages     = "1-" . $len;
        $all_pages = 1;
        $page      = 1;
    }

    if (($page - 1) > 0 && ($page - 1) <= $all_pages) {
        $pages = '<a class="pages" href="' . $url . ($page - 2) . $anchor . '">Назад</a>' . $pages;
    } else {
        $pages = 'Назад' . $pages;
    }

    /*
        if ($page > 1) {
        $pages='<a class="pages" href="'.$url.'0'.$anchor.'"><img src="'.IMG_URL.'listgo_start.gif" border="0" alt=""></a>'.$pages;
        } else {
        $pages='<img src="'.IMG_URL.'listgo_start.gif" border="0" alt="">'.$pages;
        }
    */
    if (($page + 1) <= $all_pages) {
        $pages .= '<a class="pages" href="' . $url . $page . $anchor . '">Вперед</a>';
    } else {
        $pages .= 'Вперед';
    }

    /*
        if ($page < $all_pages) {
        $pages .= '<a class="pages" href="'.$url.($all_pages - 1).$anchor.'"><img src="'.IMG_URL.'listgo_end.gif" border="0" alt=""></a>';
        } else {
        $pages .= '<img src="'.IMG_URL.'listgo_end.gif" border="0" alt="">';
        }*/

    return $pages;
}


function pages2($sum, $len, $function = "", $param = "", $param2 = "")
{
    global $page;
    $max_show_pages = 4; //кол-во цифр на странице

    if ($anchor != "") {
        $anchor = "#" . $anchor;
    }
    $all_pages = ceil($sum / $len);
    $page++;
    if ($page < 1) {
        $page = 1;
    } elseif ($page > $all_pages) {
        $page = $all_pages;
    }

    $start = $page - floor($max_show_pages / 2);
    if ($start < 1) {
        $start = 1;
    }
    if ($start > ($all_pages - $max_show_pages)) {
        $start = $all_pages - $max_show_pages;
    }
    $finish = $page + floor($max_show_pages / 2);
    if ($finish > $all_pages) {
        $finish = $all_pages;
    } elseif ($finish < $max_show_pages && $max_show_pages < $all_pages) {
        $finish = $finish + ($max_show_pages - $finish);
    }
    $pages = "";
    for ($i = $start; $i < $finish + 1; $i++) {
        if ($i > 0) {
            $link_i = (($i - 1) * $len + 1) . "-" . (($i - 1) * $len + $len);
            if ($i == $page) {
                $pages .= "<span class=\"cur_page\">&nbsp;" . $i . "&nbsp;</span>";
            } else {
                $pages .= "&nbsp;<a class=\"pages num\" href=\"" . $function . "(" . ($i - 1) . $anchor . "\">" . $i . "</a>&nbsp;";
            }
        }
    }

    if ($pages == "") {
        $pages     = "1-" . $len;
        $all_pages = 1;
        $page      = 1;
    }

    if (($page - 1) > 0 && ($page - 1) <= $all_pages) {
        $pages = '<a class="pages" href="' . $url . ($page - 2) . $anchor . '">Назад</a>' . $pages;
    } else {
        $pages = 'Назад' . $pages;
    }
    /*
        if ($page > 1) {
        $pages='<a class="pages" href="'.$url.'0'.$anchor.'"><img src="'.IMG_URL.'listgo_start.gif" border="0" alt=""></a>'.$pages;
        } else {
        $pages='<img src="'.IMG_URL.'listgo_start.gif" border="0" alt="">'.$pages;
        }
    */
    if (($page + 1) <= $all_pages) {
        $pages .= '<a class="pages" href="' . $url . $page . $anchor . '">Вперед</a>';
    } else {
        $pages .= 'Вперед';
    }

    /*
        if ($page < $all_pages) {
        $pages .= '<a class="pages" href="'.$url.($all_pages - 1).$anchor.'"><img src="'.IMG_URL.'listgo_end.gif" border="0" alt=""></a>';
        } else {
        $pages .= '<img src="'.IMG_URL.'listgo_end.gif" border="0" alt="">';
        }*/

    return $pages;
}

//Преобразуем в number
function to_number($str)
{

    $str = preg_replace("/,/", ".", (string)$str);
    $str = preg_replace("/[^0-9\.]/", "", $str);
    $str = preg_replace("/^0+/", "", $str);
    if ($str == "" || !preg_match('/^-?\d+[\.|\,]?\d+$/', $str))
        $str = "0";

    return $str;
}

//Проверяем наличие
function check_attr($name, $value = 1)
{
    global $auth;
    if ($auth->auth["admin"] == 1)
        return true;

    if (isset($auth->auth["attr"][$name])) {
        if (is_array($value)) {
            if (is_array($auth->auth["attr"][$name])) {
                foreach ($value as $v) {
                    if (in_array($v, $auth->auth["attr"][$name])) {
                        return true;
                    }
                }

                return false;
            } elseif (in_array($auth->auth["attr"][$name], $value)) {
                return true;
            } else {
                return false;
            }
        } else {
            if (is_array($auth->auth["attr"][$name])) {
                if (in_array($value, $auth->auth["attr"][$name])) {
                    return true;
                } else {
                    return false;
                }
            } elseif ($auth->auth["attr"][$name] == $value) {
                return true;
            } else {
                return false;
            }
        }
    } else {
        return false;
    }
}

### Красивое отображение номера телефона

function phone_decor($number, $type = 0)
{
    $number = preg_replace("/[^0-9]/", "", $number);
    $number = preg_replace("/^7|8([0-9]{10})/", "\\1", $number);
    if ($number) {
        if ($type == 0) {
            $number = preg_replace("|(\d{3})(\d{3})(\d+)|", "+7 ( \\1 ) \\2-\\3", $number);
        } elseif ($type == 1) {
            $number = '<font style="white-space:nowrap;">' . preg_replace("|(\d{3})(\d{3})(\d+)|", "+7(\\1)\\2-\\3", $number) . '</font>';
        }
    } else {
        $number = "-";
    }

    return $number;
}

function dot2comma($str, $prec = 1, $sep = "'")
{
    //return str_replace(".",",",$str);
    return number_format($str, $prec, ",", $sep);
}

define("TYPE_STRING", 0);
define("TYPE_INT", 1);
define("TYPE_FLOAT", 2);
define("TYPE_DATE", 3);
define("TYPE_TIME", 4);
define("IS_ARRAY", 1);
define("TO_SESS", 1);
define("TO_SESS_GET", 2);

function set_vars(
    $set_vars_arr, //имена переменных
    $set_vars_type = 0, //тип переменной
    $set_vars_to_sess = 0, //переменню необходимо сохранить в сессию, 1-post, 2-get
    $set_vars_is_array = 0, //переменная является одномерным массивом
    $set_vars_values = array() //предопределенные значения
)
{
    if (!is_array($set_vars_arr)) {
        return false;
    }
    foreach ($set_vars_arr as $value) {
        global $$value;

        if (isset($_POST[$value]))
            $var = $_POST[$value];
        elseif (isset($_GET[$value]))
            $var = $_GET[$value];
        elseif (isset($_COOKIE[$value]))
            $var = $_COOKIE[$value];
        elseif (isset($_SESSION[$value]) && !$set_vars_to_sess)
            $var = $_SESSION[$value];

        if ($set_vars_is_array == 0) {
            if (!empty($set_vars_values) && !in_array($var, $set_vars_values))
                $var = "";
            if ($set_vars_type == 1) {
                //целое
                if (isset($var)) {
                    $var = intval(preg_replace("/[ ']/", "", $var));
                } else {
                    $var = 0;
                }
            } elseif ($set_vars_type == 2) {
                //дробное
                if (isset($var)) {
                    $var = floatval(preg_replace("/,/", ".", preg_replace("/[ ']/", "", $var)));
                } else {
                    $var = 0;
                }
            } elseif ($set_vars_type == 3) {
                //дата
                if (!isset($var) || !currdate($var)) {
                    $var = "";
                }
            } elseif ($set_vars_type == 4) {
                //время
                if (!isset($var) || !currtime($var)) {
                    $var = "";
                }
            } else {
                //строка
                if (isset($var)) {
                    $var = checkstr($var);
                } else {
                    $var = "";
                }
            }
        } else {
            if (isset($var) && is_array($var)) {
                $tmp = array();
                foreach ($var as $k => $v) {
                    $k = checkstr($k);
                    if (!empty($set_vars_values) && !in_array($v, $set_vars_values))
                        $v = "";
                    if ($set_vars_type == 1) {
                        //целое
                        $v = intval(preg_replace("/[ ']/", "", $v));
                    } elseif ($set_vars_type == 2) {
                        //дробное
                        $v = floatval(preg_replace("/,/", ".", preg_replace("/[ ']/", "", $v)));
                    } elseif ($set_vars_type == 3) {
                        //дата
                        if (!currdate($v))
                            $v = "";
                    } elseif ($set_vars_type == 4) {
                        //время
                        if (!currtime($v))
                            $v = "";
                    } else {
                        //строка
                        $v = checkstr($v);
                    }
                    $tmp[$k] = $v;
                }
                if (isset($_SESSION[$value]) && is_array($_SESSION[$value])) {
                    //если в сессии есть значения, то добавляем их
                    //$var=array_merge($_SESSION[$value], $tmp); //нах ??
                } else {
                    $var = $tmp;
                }
                unset($tmp);
            } else {
                $var = array();
            }
        }


        if ($set_vars_to_sess == 1) {
            # сохраняем POST данные в сессии
            if (isset($_POST[$value])) {
                $_SESSION[$value] = $var;
            } else {
                $_SESSION[$value] = null;
                unset($_SESSION[$value]);
            }
        } elseif ($set_vars_to_sess == 2) {
            # сохраняем GET данные в сессии
            if (isset($_GET[$value])) {
                $_SESSION[$value] = $var;
            } else {
                unset($_SESSION[$value]);
            }
        }
        $$value = $var;
        unset($var);
    }

    return;
}

//переводит числовое отображение еденицы времени в строковое (9 -> &nbsp;&nbsp;9&nbsp;часов)
function time2string($number, $type)
{
    $ret    = "";
    $number = intval($number);
    if ($number < 10)
        $ret = "&nbsp;&nbsp;" . $number;
    elseif ($number < 100)
        $ret = "&nbsp;" . $number;

    switch ($type) {
        case "day":
            if ($number >= 10 && $number <= 20)
                $ret .= "&nbsp;дней";
            elseif (substr($number, -1) == "1")
                $ret .= "&nbsp;день";
            elseif (substr($number, -1) >= 2 && substr($number, -1) <= 4)
                $ret .= "&nbsp;дня&nbsp;";
            else
                $ret .= "&nbsp;дней";
            break;
        case "hour":
            if ($number >= 10 && $number <= 20)
                $ret .= "&nbsp;часов";
            elseif (substr($number, -1) == "1")
                $ret .= "&nbsp;час&nbsp;&nbsp;";
            elseif (substr($number, -1) >= 2 && substr($number, -1) <= 4)
                $ret .= "&nbsp;часа&nbsp;";
            else
                $ret .= "&nbsp;часов";
            break;
        case "minut":
            if ($number >= 10 && $number <= 20)
                $ret .= "&nbsp;минут";
            elseif (substr($number, -1) == "1")
                $ret .= "&nbsp;минуту";
            elseif (substr($number, -1) >= 2 && substr($number, -1) <= 4)
                $ret .= "&nbsp;минуты";
            else
                $ret .= "&nbsp;минут";
            break;
    }

    return $ret;
}

//проверяет, что массив не пуст
function array_full($arr)
{
    if (is_array($arr)) {
        foreach ($arr as &$v) {
            if (!empty($v))
                return true;
        }
    }

    return false;
}

function nl2br2($str)
{
    $str = preg_replace("/[\n\r\t]/", "", nl2br($str));

    return $str;
}

function br2nl($str)
{
    $str = preg_replace("/[\n\r\t]/", "", $str);
    $str = preg_replace("/<br \/>|<br>/", "\n", $str);

    return $str;
    //return preg_replace('/\<br(\s*)?\/?\>/i', "\n", $str);
}

function add_br($str)
{
    if (strlen($str) > 64) {
        return preg_replace('/}/', '}
', $str);
    }

    return $str;
}


function color_tone($color, $tone = .8)
{
    $r = hexdec(substr($color, 1, 2));
    $g = hexdec(substr($color, 3, 2));
    $b = hexdec(substr($color, 5, 2));

    return "#" . dechex($r * $tone) . dechex($g * $tone) . dechex($b * $tone);
}

function check_interval($date_start, $date_end, $max_day)
{
    //проверка, что в интервал входит определенное кол-во дней
    $diff = datedigit($date_end, 1) - datedigit($date_start, 1);
    if ($diff >= 0 && $diff <= 86400 * $max_day) {
        return true;
    } else {
        return false;
    }
}


function num2str($num)
{
    $nul     = 'ноль';
    $ten     = array(
        array('', 'один', 'два', 'три', 'четыре', 'пять', 'шесть', 'семь', 'восемь', 'девять'),
        array('', 'одна', 'две', 'три', 'четыре', 'пять', 'шесть', 'семь', 'восемь', 'девять'),
    );
    $a20     = array('десять', 'одиннадцать', 'двенадцать', 'тринадцать', 'четырнадцать', 'пятнадцать', 'шестнадцать', 'семнадцать', 'восемнадцать', 'девятнадцать');
    $tens    = array(2 => 'двадцать', 'тридцать', 'сорок', 'пятьдесят', 'шестьдесят', 'семьдесят', 'восемьдесят', 'девяносто');
    $hundred = array('', 'сто', 'двести', 'триста', 'четыреста', 'пятьсот', 'шестьсот', 'семьсот', 'восемьсот', 'девятьсот');
    $unit    = array(// Units
                     array('копейка', 'копейки', 'копеек', 1),
                     array('рубль', 'рубля', 'рублей', 0),
                     array('тысяча', 'тысячи', 'тысяч', 1),
                     array('миллион', 'миллиона', 'миллионов', 0),
                     array('миллиард', 'милиарда', 'миллиардов', 0),
    );
    //
    list($rub, $kop) = explode('.', sprintf("%015.2f", floatval($num)));
    $out = array();
    if (intval($rub) > 0) {
        foreach (str_split($rub, 3) as $uk => $v) { // by 3 symbols
            if (!intval($v))
                continue;
            $uk     = sizeof($unit) - $uk - 1; // unit key
            $gender = $unit[$uk][3];
            list($i1, $i2, $i3) = array_map('intval', str_split($v, 1));
            // mega-logic
            $out[] = $hundred[$i1]; # 1xx-9xx
            if ($i2 > 1)
                $out[] = $tens[$i2] . ' ' . $ten[$gender][$i3];# 20-99
            else
                $out[] = $i2 > 0 ? $a20[$i3] : $ten[$gender][$i3];# 10-19 | 1-9
            // units without rub & kop
            if ($uk > 1)
                $out[] = morph($v, $unit[$uk][0], $unit[$uk][1], $unit[$uk][2]);
        } //foreach
    } else
        $out[] = $nul;
    $out[] = morph(intval($rub), $unit[1][0], $unit[1][1], $unit[1][2]); // rub
    $out[] = $kop . ' ' . morph($kop, $unit[0][0], $unit[0][1], $unit[0][2]); // kop
    return trim(preg_replace('/ {2,}/', ' ', join(' ', $out)));
}

/**
 * Склоняем словоформу
 * @ author runcore
 */
function morph($n, $f1, $f2, $f5)
{
    $n = abs(intval($n)) % 100;
    if ($n > 10 && $n < 20)
        return $f5;
    $n = $n % 10;
    if ($n > 1 && $n < 5)
        return $f2;
    if ($n == 1)
        return $f1;

    return $f5;
}

function to_money($number)
{
    $number = number_format($number, 2, '.', ' ');

    return $number;
}

/* function SendEmail($to, $subject, $mess, $from="KC Server <nobody@contact.orient.root.biz>") {
  if(isset($_SERVER["SERVER_NAME"]) && $_SERVER["SERVER_NAME"]=="kc") {

  mail($to,$subject,$mess, "Content-Type: text/plain; charset=windows-1251\nFrom: ".$from."\n");
  } else {
  $to = preg_replace("/@dvtb.ru/","@orient.root.biz",$to);

  $smtp = new SMTP;
  $mail = explode(",", $to);
  foreach($mail as $m) {
  $smtp->send($m, $subject, $mess);
  }

  Log::get("mail", $to, $subject, $mess);
  }
  }


  /*function SendEmail($to, $subject, $mess, $from="KC Server <nobody@contact.orient.root.biz>") {
  if(isset($_SERVER["SERVER_NAME"]) && $_SERVER["SERVER_NAME"]=="kc") {
  mail($to,$subject,$mess, "Content-Type: text/plain; charset=windows-1251\nFrom: ".$from."\n");
  } else {
  $to=preg_replace("/@dvtb.ru/","@orient.root.biz",$to);
  $hdrs = array(
  "From"      => $from,
  "To"        => $to,
  "Subject"   => $subject
  );
  $mime = new Mail_mime("\n");
  $mime -> setTXTBody($mess);
  $body = $mime->get();
  $hdrs = $mime->headers($hdrs);
  $params = array("host" => "192.168.132.173", "port" => "25");
  $mail = Mail::factory("smtp",$params);

  if($mail->send($to,$hdrs,$body)) {
  Log::get("mail", $to, $subject, $mess);
  } else {
  Log::get("mail", "Error=".$to, $subject, $mess);
  }
  }
  } */

function set_deal_services($deal_id)
{
    global $dbo, $mdb;

    $dbo->query("SELECT
		  d2t.* ,
		  c.name country_name,
		  ct.name city_name,
		  h.name hotel_name,
		  pt.full_name full_place_type_name,
		  pt.short_name place_type_name,
		  rt.short_name room_type_name,
		  rt.full_name full_room_type_name,
		  ft.short_name food_type_name,
		  ft.full_name full_food_type_name
		FROM
		  deals2tours d2t,
		  " . $mdb . ".countries c,
		  " . $mdb . ".cities ct,
		  " . $mdb . ".hotels h,
		  " . $mdb . ".placing_types pt,
          " . $mdb . ".food_types ft,
          " . $mdb . ".room_types rt
		WHERE
		   d2t.country_id = c.country_id
		  AND d2t.city_id = ct.city_id
		  AND d2t.hotel_id = h.hotel_id
		  AND d2t.room_type = rt.type_id
          AND d2t.food_type = ft.type_id
          AND d2t.place_type = pt.type_id
		  and d2t.deal_id =" . $deal_id);
    while ($dbo->next_record()) {
        $result[$dbo["tour_id"]]["service_type"]         = 1;
        $result[$dbo["tour_id"]]["name"]                 = $dbo["hotel_name"] . "(" . $dbo["country_name"] . ", " . $dbo["city_name"] . ")";
        $result[$dbo["tour_id"]]["hotel_name"]           = $dbo["hotel_name"];
        $result[$dbo["tour_id"]]["country_name"]         = $dbo["country_name"];
        $result[$dbo["tour_id"]]["city_name"]            = $dbo["city_name"];
        $result[$dbo["tour_id"]]["hotel_rooms"]          = $dbo["rooms"];
        $result[$dbo["tour_id"]]["room_type_name"]       = $dbo["room_type_name"];
        $result[$dbo["tour_id"]]["place_type_name"]      = $dbo["place_type_name"];
        $result[$dbo["tour_id"]]["food_type_name"]       = $dbo["food_type_name"];
        $result[$dbo["tour_id"]]["full_room_type_name"]  = $dbo["full_room_type_name"];
        $result[$dbo["tour_id"]]["full_place_type_name"] = $dbo["full_place_type_name"];
        $result[$dbo["tour_id"]]["full_food_type_name"]  = $dbo["full_food_type_name"];
        $result[$dbo["tour_id"]]["deal_start_date"]      = $dbo["start_date"];
        $result[$dbo["tour_id"]]["deal_end_date"]        = $dbo["end_date"];
        $result[$dbo["tour_id"]]["tour_nights"]          = $dbo["nights"];


        /*
                $return["hotel_name"]          = $services[$hotel_id]["hotel_name"];
                $return["hotel_country"]       = $services[$hotel_id]["arrival_country"];
                $return["hotel_city"]          = $services[$hotel_id]["arrival_city"];
                $return["hotel_rooms"]         = $services[$hotel_id]["hotel_rooms"];
                $return["hotel_roomType"]      = $services[$hotel_id]["room_type_name"];
                $return["hotel_placeType"]     = $services[$hotel_id]["place_type_name"];
                $return["hotel_foodType"]      = $services[$hotel_id]["food_type_name"];
                $return["hotel_fullRoomType"]  = $services[$hotel_id]["full_room_type_name"];
                $return["hotel_fullPlaceType"] = $services[$hotel_id]["full_place_type_name"];
                $return["hotel_fullFoodType"]  = $services[$hotel_id]["full_food_type_name"];
                $return["hotel_dateBegin"]     = $services[$hotel_id]["deal_start_date"];
                $return["hotel_dateEnd"]       = $services[$hotel_id]["deal_end_date"];
                $return["hotel_nights"]        = $services[$hotel_id]["tour_nights"];

        */
    }
    $dbo->query("SELECT
			  d2t.*,
			  da.name d_airport_name,
			  dct.name d_city_name,
			  aa.name a_airport_name,
			  act.name a_city_name
			FROM
			  deals2tickets d2t,
			  " . $mdb . ".airports da,
			  " . $mdb . ".cities dct,
			  " . $mdb . ".airports aa,
			  " . $mdb . ".cities act
			WHERE d2t.departure_airport_id = da.airport_id
			AND   d2t.departure_city_id = dct.city_id
			  AND d2t.arrival_airport_id = aa.airport_id
			  AND d2t.arrival_city_id = act.city_id
			  AND d2t.deal_id = " . $deal_id);
    while ($dbo->next_record()) {
        echo 1;
        $result[$dbo["ticket_id"]]["start_date"]   = $dbo["start_date"];
        $result[$dbo["ticket_id"]]["name"]         = $dbo["d_airport_name"] . "(" . $dbo["d_city_name"] . ") - " . $dbo["a_airport_name"] . "(" . $dbo["a_city_name"] . ")";
        $result[$dbo["ticket_id"]]["service_type"] = 3;


    }
    $dbo->query("SELECT
		  d2h.* ,
		  c.name country_name,
		  ct.name city_name,
		  h.name hotel_name
		FROM
		  deals2hotels d2h,
		  " . $mdb . ".countries c,
		  " . $mdb . ".cities ct,
		  " . $mdb . ".hotels h
		WHERE  d2h.country_id = c.country_id
		  AND d2h.city_id = ct.city_id
		  AND d2h.hotel_id = h.hotel_id AND d2h.deal_id =" . $deal_id);
    while ($dbo->next_record()) {
        $result[$dbo["hotel_id"]]["start_date"]   = $dbo["start_date"];
        $result[$dbo["hotel_id"]]["name"]         = $dbo["hotel_name"] . "(" . $dbo["country_name"] . ", " . $dbo["city_name"] . ")";
        $result[$dbo["hotel_id"]]["service_type"] = 2;


    }
    $dbo->query("select * from deals2visas where deal_id=" . $deal_id);
    while ($dbo->next_record()) {
        $result[$dbo["visa_id"]]["start_date"]   = $dbo["start_date"];
        $result[$dbo["visa_id"]]["service_type"] = 4;

    }

    return $result;
}

/**
 * @param $phone_num
 * @return string
 */
function createPhoneNumber($phone_num)
{
    if (strlen($phone_num) >= 10)
        $phone_num = "+7 (" . substr($phone_num, 0, 3) . ") " . substr($phone_num, 3, 3) . "-" . substr($phone_num, 6, 2) . "-" . substr($phone_num, 8, 2);
    if (strlen($phone_num) < 10 && strlen($phone_num) > 0)
        $phone_num = substr($phone_num, strlen($phone_num) - 6, 3) . "-" . substr($phone_num, strlen($phone_num) - 4, 2) . "-" . substr($phone_num, strlen($phone_num) - 2, 2);

    return $phone_num;
}

function createPhoneToSave($phone_num)
{
    if (strlen($phone_num) > 0) $phone_num = str_replace(array('+7', '+', ' ', '(', ')', '-'), '', $phone_num);
    if (in_array(substr($phone_num, 0, 1), array(7, 8))) $phone_num = substr($phone_num, 1);

    return $phone_num;

}

function getClientTree($parent_card, $lvl, $tree, $api)
{

    global $lvl;
    global $connect_param;
    global $p;
    global $cnt;
    global $referal_array;
    global $referal_json;


    $lvl++;
    $pre = "";
    for ($i = 0; $i < $lvl; $i++) {
        $pre .= "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
    }
    $dbo_arr[$parent_card] = new DB_MySQL($connect_param);
    $dbo_arr[$parent_card]->query("SELECT c.client_id, CONCAT(c.fname,' ',c.lname) fio, card_num,parent_card_num,cr.balance, (select count(1) from clients c1 where c1.parent_card_num = c.card_num) referal_child
                  FROM clients c LEFT JOIN clients_referal cr ON c.client_id = cr.client_id  WHERE  parent_card_num = " . $parent_card . "  
                   AND EXISTS
  (SELECT
    *
  FROM
    clients_referal_balls rb,
    deals2pays dp
  WHERE rb.pay_id = dp.pay_id
  and c.deleted = 0
    AND dp.payer_id = c.client_id)");


    if ($dbo_arr[$parent_card]->num_rows() > 0) {
        if ($referal_json == "") $referal_json = "{\"referals\":{";
        else $referal_json .= ",\"referals\":{";
        while ($dbo_arr[$parent_card]->next_record()) {
            $cnt++;
            $lvl_cnt++;
            if ($api == 1) {
                if (substr($referal_json, -1) == "}") $referal_json .= ",";
                $referal_json .= "\"" . $dbo_arr[$parent_card]["card_num"] . "\":{\"fio\":\"" . $dbo_arr[$parent_card]["fio"] . "\"";
                $referal_array[$parent_card][$dbo_arr[$parent_card]["card_num"]] = $pre . $dbo_arr[$parent_card]["fio"];


            } else {


                $p->set_var(array(

                                "REFERAL_NUM"    => $dbo_arr[$parent_card]["client_id"],
                                "REFERAL_LEVEL"  => $lvl,
                                "REFERAL_CHILD"  => $dbo_arr[$parent_card]["referal_child"],
                                "REFERAL_FIO"    => $pre . $dbo_arr[$parent_card]["fio"],
                                "REFERAL_CARD"   => $dbo_arr[$parent_card]["card_num"],
                                "REFERAL_PARENT" => $dbo_arr[$parent_card]["parent_card_num"],
                                "REFERAL_SUMMA"  => $dbo_arr[$parent_card]["balance"],
                            ));
                //$p->parse("crl", "client_referal_list", true);
            }
            //{"waiting_ball":{"sum":12145,"2966":{"create_date":"19.05.2017","balls":"360","apply_date":"19.07.2017"}


            if ($dbo_arr[$parent_card]["card_num"] > 0) getClientTree($dbo_arr[$parent_card]["card_num"], $lvl, null, $api);
            $lvl--;
            $referal_json .= "}";
        }

        if ($lvl > 1) $referal_json .= "}";
    }
    $dbo_arr[$parent_card]->clear();

    if ($lvl == 1) $referal_json .= "}";

    return $cnt;
}

function getClientParents($client_id, $lvl, $summa)
{

    global $lvl;
    global $dbo;
    global $p;
    global $referal;
    global $auth;

    $lvl++;
    $pre = "";


    $dbo->query("SELECT cp.client_id FROM clients c LEFT JOIN clients cp ON c.parent_card_num = cp.card_num
                              WHERE c.parent_card_num > 0 and c.client_id =" . $client_id);


    if ($dbo->next_record() && $dbo["client_id"] > 0) {

        if ($dbo["client_id"] != 5875) {
            $referal[$dbo["client_id"]] = $summa * 0.2;
            getClientParents($dbo["client_id"], $lvl, $summa * 0.8);
        } else {
            $referal[$dbo["client_id"]] = $summa;

            return ($referal);
        }


    } else
        return ($referal);


}

/*
function GetReferalList($card_num,$level){
    global $connect_param;
    global $result;
    //echo $card_num." ";
    $result["html"].= "<div id=" . $card_num;
    if($level==1) $result["html"].= " class=referal_show";
    else $result["html"].= " class=referal_hide";
    $result["html"].=" ><ul>";

    $dbo_arr[$card_num] = new DB_MySQL($connect_param);

    $dbo_arr[$card_num]->query("SELECT c.client_id, CONCAT(c.fname,' ',c.lname) fio, card_num,parent_card_num,cr.balance, (select count(1) from clients c1 where c1.parent_card_num = c.card_num) referal_child
                  FROM clients c LEFT JOIN clients_referal cr ON c.client_id = cr.client_id  WHERE  parent_card_num = " . $card_num . "
                    limit 0,100
                 ");
    while($dbo_arr[$card_num]->next_record()){

        $result["html"].="<li class=ref_list card_num=".$dbo_arr[$card_num]["card_num"].">".$dbo_arr[$card_num]["fio"]."(".$dbo_arr[$card_num]["referal_child"].")</li>";
        if($dbo_arr[$card_num]["referal_child"]>0) {
            $result["html"].="<span id=s_".$dbo_arr[$card_num]["card_num"]." class=\"hideicon hide\"></span>";
            GetRegeralList($dbo_arr[$card_num]["card_num"], $level+1);
            echo $dbo_arr[$card_num]["fio"];
        }

    }

    //  echo mb_convert_encoding($dbo_arr[$card_num]["fio"],mb_detect_encoding($dbo_arr[$card_num]["fio"],"UTF-8"));
    $result["html"].="</ul></div>";
    $result["result"] = 1;


}*/

function translit($s)
{
    $s = (string)$s; // преобразуем в строковое значение
    $s = strip_tags($s); // убираем HTML-теги
    $s = str_replace(array("\n", "\r"), " ", $s); // убираем перевод каретки
    $s = preg_replace("/\s+/", ' ', $s); // удаляем повторяющие пробелы
    $s = trim($s); // убираем пробелы в начале и конце строки

    $s = strtr($s, array(
        'а' => 'a', 'б' => 'b', 'в' => 'v', 'г' => 'g', 'д' => 'd',
        'е' => 'e', 'ё' => 'yo', 'ж' => 'zh', 'з' => 'z', 'и' => 'i',
        'й' => 'y', 'к' => 'k', 'л' => 'l', 'м' => 'm', 'н' => 'n',
        'о' => 'o', 'п' => 'p', 'р' => 'r', 'с' => 's', 'т' => 't',
        'у' => 'u', 'ф' => 'f', 'х' => 'h', 'ц' => 'ch', 'ч' => 'c',
        'ш' => 'sh', 'щ' => 'shch', 'ы' => 'y', 'э' => 'e',
        'ю' => 'yu', 'я' => 'ya', 'ъ' => '', 'ь' => '',
        'А' => 'A', 'Б' => 'B', 'В' => 'V', 'Г' => 'G', 'Д' => 'D',
        'Е' => 'E', 'Ё' => 'YO', 'Ж' => 'Zh', 'З' => 'Z', 'И' => 'I',
        'Й' => 'Y', 'К' => 'K', 'Л' => 'L', 'М' => 'M', 'Н' => 'N',
        'О' => 'O', 'П' => 'P', 'Р' => 'R', 'С' => 'S', 'Т' => 'T',
        'У' => 'U', 'Ф' => 'F', 'Х' => 'H', 'Ц' => 'CH', 'Ч' => 'C',
        'Ш' => 'Sh', 'Щ' => 'Shch', 'Ы' => 'Y', 'Э' => 'E',
        'Ю' => 'Yu', 'Я' => 'Ya', 'Ъ' => '', 'Ь' => ''));
    $s = preg_replace("/[^0-9a-z-_ ]/i", "", $s); // очищаем строку от недопустимых символов

    return $s; // возвращаем результат
}


?>