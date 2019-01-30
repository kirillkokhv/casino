<?php
# последни изменения 07.04.2009

class DB_MySQL implements ArrayAccess {

  protected $Construct     = 0;
  protected $Distruct	     = 1;
  protected $PConnect	     = 0;

  public    $Host          = "localhost";
  public    $Database      = "tur";
  public    $User          = "root";    //привелегия только для авторизации
  public    $Password      = "";
  public    $Port          = "3306";

  protected $LogErr	       = 1;	        // Лог ошибок : libs , connect
  protected $LogQuery	     = 1;         // Лог запросов: select,update,insert,delete,procedure
  protected $ShowErr	     = 0;         // 0 - не отображать ошибку, 1 - html, 3 - plain text
  protected $ExitErr	     = 1;         // выход при ошибке
  
  public    $Record        = array();   //массив с результатами запроса
  public    $Row;                       //id строки

  protected $RecCount      = 0;		      // Кол-во записей в текущем курсоре
  protected $ResFirst      = array();   // Первая запись в запросе, для check_row

  public    $Link_ID       = 0;         //id соединения
  protected $Query_ID      = 0;         //id запроса
  public    $Query         = "";		     // Строка запроса

  public    $SessArgs      = array("NAMES CP1251"); //Переменные сессии
  
  protected $Errno         = 0;         //код ошибки
  protected $Error         = "";        //текст ошибки
  

  public function __construct($param=array()) {
    foreach($param as $n=>$p) $this->$n=$p;
    if($this->Construct==1) @$this->connect();
  }

  //Очищаем переменныяе (если объект вытащили из сессии)
  public function clear() {
    $this->Link_ID=0;
    $this->Query_ID=0;
    $this->Query="";
    $this->Errno=0;
    $this->Error="";
    $this->Record=array();
  }
  
  //Получаем новые переменные сессии
  public function add_arg_session($args){
    if(is_array($args))  $this->SessArgs = array_merge ($this->SessArgs, $args);
    elseif(is_string($args)) $this->SessArgs[] = $args;
    
    if ( $this->Link_ID ) { //коннект был
      if(is_array($args))  $this->set_session($args);
      elseif(is_string($args)) $this->set_session(array($args));
    }
  }

  // Выставляем переменные сессии
  public function set_session($args){
      $sqlstr="SET ";
      foreach($args as $value) {
        if(!strstr($sqlstr,$value)) $sqlstr.=$value.",";
      }
      $sqlstr=substr($sqlstr,0,strlen($sqlstr)-1); //убираем лишнюю запятую
      @mysql_query($sqlstr,$this->Link_ID);
  }
  
  // Коннект
  public function connect() {
    if ( 0 == $this->Link_ID ) {

      if($this->PConnect) {
        $this->Link_ID = @mysql_pconnect($this->Host.":".$this->Port, $this->User, $this->Password);
      } else {
        $this->Link_ID = @mysql_connect($this->Host.":".$this->Port, $this->User, $this->Password);
      }
      
      if (!$this->Link_ID) {
        $this->halt("CONN",mysql_error()." # Connect (".$this->Host.", ".$this->User.", ".$this->Password.") failed");
        return 0;
      }
      if(!empty($this->Database))
      {
        if (!@mysql_select_db($this->Database,$this->Link_ID)) {
          $this->halt("USEDB","cannot use database ".$this->Database);
          return 0;
        }
      }
      $this->set_session($this->SessArgs); //переменные сессии
    }
    return $this->Link_ID;
  }

  // Реконнект
  public function reconnect() {
    $this->disconnect($this->Link_ID);
    return $this->connect();
  }

  // Выполнение запроса (select,update,insert)
  public function query($Query_String, $start=0, $end=0) {
    $sTime=microtime(1); //счетчик времени запроса
    $this->Record=array();
    if ($Query_String == "") return 0;
    $this->Query=$Query_String;
    
    if (!$this->Link_ID) $this->connect();  //коннекта не было
    if ($start>=0 && $end>0) $Query_String.=" LIMIT ".$start.", ".$end;
    
    
    
    $this->Query_ID = @mysql_query($Query_String,$this->Link_ID);
    
    $this->Row = 0;
		
    if (!$this->Query_ID) {
		
		if($this->LogQuery == 1) Log::get("mysql", "QUERY", round((microtime(1)-$sTime),2), "Error = ".$Query_String);
		//if($this->LogQuery == 1) Log::get("error", "MYSQL", round((microtime(1)-$sTime),2), "Invalid SQL: ".$Query_String);
		$this->halt("QUERY","Invalid SQL: ".$Query_String);
		return false;
    } else {
      if($this->LogQuery == 1) Log::get("mysql", "QUERY", round((microtime(1)-$sTime),2), $Query_String);
    }
	
	  	    
    global $TIME_SQL;
	    if(empty($TIME_SQL)) $TIME_SQL=0;
	    $TIME_SQL+=round((microtime(1)-$sTime),4);
    
    return $this->Query_ID;
  }

  //проверка наличия записей
  public function check_rows() {
  	$this->RecCount = false;
    $this->ResFirst = @mysql_fetch_assoc($this->Query_ID);
    if(!empty($this->ResFirst)){
  		$this->RecCount = true;
  	}
  	return $this->RecCount;
  }

  //смещение курсора
  public function next_record() {
    if (!$this->Query_ID) {
      $this->halt("NXTRE","next_record called with no query pending.");
      return 0;
    }

  	if(!empty($this->ResFirst)){
  	    $this->Record = $this->ResFirst;
  	    unset($this->ResFirst);
  	}elseif(!empty($this->Query_ID) && $this->num_rows()>0){
  	    $this->Record = @mysql_fetch_assoc($this->Query_ID);
  	}else{
      $this->Record = false;
    }

    $this->Row += 1;
  	return is_array($this->Record);
  }


  //перемещает внутренний указатель в результате запроса
  public function seek($pos = 0) {
    $status = @mysql_data_seek($this->Query_ID, $pos);
    if ($status){
      $this->Row = $pos;
    } else {
      $this->halt("SEEK","seek($pos) failed: result has ".$this->num_rows()." rows.");

      @mysql_data_seek($this->Query_ID, $this->num_rows());
      $this->Row = $this->num_rows();
      return 0;
    }

    return 1;
  }

  //возвращает число затронуиых прошлой операцией рядов (insert, update, delete)
  public function affected_rows() {
    return @mysql_affected_rows($this->Link_ID);
  }

  //количество записей
  public function num_rows() {
    return @mysql_num_rows($this->Query_ID);
  }

  //количество столбцов
  public function num_fields() {
    return @mysql_num_fields($this->Query_ID);
  }

  //ассоциированный массив результата
  public function f($Name) {
    if (isset($this->Record[$Name])) {
      return $this->Record[$Name];
    }
    return "";
  }

  //последовательность
  public function last_id() {
    if ($this->Link_ID) {
       return mysql_insert_id($this->Link_ID);
     }
     return 0;
  }


  //ошибка
  private function halt($type,$msg) {
	
    $this->Error = mysql_error($this->Link_ID);
	
    $this->Errno = mysql_errno($this->Link_ID);

	
    
		

    $this->haltmsg($msg);
  }

  private function haltmsg($msg) {
    
      printf("<p><b>Database error:</b> %s<br>\n", $msg);
      printf("<b>MySQL Error</b>: %s (%s)</p>\n",  $this->Errno, $this->Error);
    
  
  	 echo '<html><head><meta http-equiv="Content-Type" content="text/html; charset=windows-1251"></head>';
  	 echo '<body><center><br><br><font color="red"><h3><b>Ошибка при подключении к базе данных!</b></h3><br><h4>Попробуйте повторить запрос нажав <b>F5</b></h4></font></body></html>';
  	
  	if($this->ExitErr) exit;
  }
  
  // Разрыв соединения
  public function disconnect() {
    if ($this->Link_ID) {
        mysql_close($this->Link_ID);
        $this->Link_ID=0;
       return 1;
     };
     return 0;
  }


  public function __destruct() {
    $this->Record=array();
    if($this->Distruct==1) @$this->disconnect();
  }

  public function offsetGet($name){
    return $this->f($name);
  }
  public function offsetExists($name){
    if (isset($this->Record[$name])) {
      return true;
    } else {
      return false;
    }
  }
  public function offsetSet($name,$val){
    $this->Record[$name]=$val;
    return true;
  }
  public function offsetUnset($name){
    unset($this->Record[$name]);
    return true;
  }
  
  private function get_error() {
    	if(0 == $this->Query_ID) {
      		$error = oci_error();
    	} else {
      		$error = oci_error($this->Query_ID);
    	}
    	if($error["code"]!=0 && $error["code"]!=1403) {
      		$this->Errno = $error["code"];
      		$this->Error = $error["message"];
    	} else {
      		$this->Errno = 0;
      		$this->Error = "";
    	}
  	}

}


class DB_MySQL_cron extends DB_MySQL {
  public $User     = "cron";
  public $Password = "z9QvS6vB2hQEXKQJ";
  public $LogQuery = 0;
  public $ShowErr  = 2;
}

?>
