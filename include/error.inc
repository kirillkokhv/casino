<?php
$title_error[-1]='Регламентные работы';
$title_error[400]='Bad request!';
$title_error[401]='Authentication required!';$title_error[403]='Требуется авторизация!';
$title_error[403]='Access forbidden!';$title_error[403]='Доступ запрещен!';
$title_error[404]='Object not found!';$title_error[404]='Объект не найден!';
$title_error[405]='Method not allowed!';
$title_error[408]='Request time-out!';
$title_error[410]='Resource is no longer available!';
$title_error[411]='Bad Content-Length!';
$title_error[412]='Precondition failed!';
$title_error[413]='Request entity too large!';
$title_error[414]='Submitted URI too large!';
$title_error[415]='Unsupported media type!';
$title_error[500]='Server error!';$title_error[500]='Ошибка сервера!';
$title_error[501]='Cannot process request!';
$title_error[502]='Bad Gateway!';
$title_error[503]='Service unavailable!';
$title_error[506]='Variant also varies!';

if($deep2=="" || empty($title_error[$deep2])){ $deep2=404; }
$p->set_var("ERROR", $deep2." - ".$title_error[$deep2]);
?>