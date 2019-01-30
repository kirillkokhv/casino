function getXmlHttp() {
    var xmlhttp;
    try {
        xmlhttp = new ActiveXObject("Msxml2.XMLHTTP");
    } catch (e) {
        try {
            xmlhttp = new ActiveXObject("Microsoft.XMLHTTP");
        } catch (E) {
            xmlhttp = false;
        }
    }
    if (!xmlhttp && typeof XMLHttpRequest != 'undefined') {
        xmlhttp = new XMLHttpRequest();
    }
    return xmlhttp;
}

function set_payer(deal_id, client_id) {

    // (1) ������� ������ ��� ������� � �������
    var req = getXmlHttp();

    req.onreadystatechange = function () {
        // onreadystatechange ������������ ��� ��������� ������ �������

        if (req.readyState == 4) {
            // ���� ������ �������� �����������


        }

    }

    // (3) ������ ����� �����������
    req.open('POST', './?detect=set_payer&deal_id=' + deal_id + '&client_id=' + client_id, true);


    // (4)
    req.send(null);  // �������� ������


}
function win_update(win) {
    win.hidden = true;
    win.afterHide();
    win.afterShow(false);
}

function timespent_getSecs() {
    var s1 = parseInt(timespent_initStopwatch(), 10);
    var m = parseInt(s1 / 60, 10);
    s = s1 - m * 60;
    s = s + " ���.";
    if (m > 0) {
        m = m + " ���. ";
    } else {
        m = "";
    }
    txt = m + s;
    if (s1 >= max_time * 60) {
        document.all.timespent.color = "red";
        txt = "<b>" + txt + "</b>";
    }
    document.all.timespent.innerHTML = txt;
    window.setTimeout("timespent_getSecs()", 1000);
}

function timespent_initStopwatch() {
    var myTime = new Date();
    var timeNow = myTime.getTime();
    var timeDiff = timeNow - clockStart + time_sa;
    set_cookie("time_sa", timeDiff, 1);
    this.diffSecs = timeDiff / 1000;
    return (this.diffSecs);
}

function truncate(str, len) {
    if (str.length > len) {
        str = str.substring(0, len - 3) + '...';
    }
    return str;
}

function sp_atributs_show(t) {
    var td = t.parentNode;
    td = td.childNodes;
    for (i = 0; i < td.length; i++) {
        if (td[i].tagName == 'DIV') {
            td[i].style.display = '';
            break;
        }
    }
}
function sp_atributs_select(t) {
    t.parentNode.style.display = 'none';
    var ret = '';
    if (t.value == 'city_id') {
        var data = new Array();
        var url = '/dialogs/address/?city=1';
        var data = window.open(url, null, "status:no;dialogWidth:400px;scroll:no;dialogHeight:400px;help:no;");
        if (typeof(data) != "undefined" && typeof(data["city_id"]) != "undefined") {
            ret = data["city_id"];
        }
    }

    if (t.value == 'region_id') {
        var data = new Array();
        var url = '/dialogs/address/?region_id=1';
        var data = window.open(url, null, "status:no;dialogWidth:400px;scroll:no;dialogHeight:400px;help:no;");
        if (typeof(data) != "undefined" && typeof(data["region_id"]) != "undefined") {
            ret = data["region_id"];
        }
    }

    if (t.value == 'office_id') {
        var data = new Array();
        var url = '/dialogs/offices/?region_id=1';
        var data = window.open(url, null, "status:no;dialogWidth:900px;scroll:yes;dialogHeight:600px;help:no;");
        if (typeof(data) != "undefined" && typeof(data["office_id"]) != "undefined") {
            ret = data["office_id"];
        }
    }

    if (t.value == 'filial_id') {
        var data = new Array();
        var url = '/dialogs/filials/?filial_id=1';
        var data = window.open(url, null, "status:no;dialogWidth:400px;scroll:no;dialogHeight:400px;help:no;");
        if (typeof(data) != "undefined" && typeof(data["filial_id"]) != "undefined") {
            ret = data["filial_id"];
        }
    }

    if (t.value == 'territory_id') {
        var data = new Array();
        var url = '/dialogs/territory/?territory_id=1';
        var data = window.open(url, null, "status:no;dialogWidth:400px;scroll:no;dialogHeight:400px;help:no;");
        if (typeof(data) != "undefined" && typeof(data["territory_id"]) != "undefined") {
            ret = data["territory_id"];
        }
    }

    var tr = t.parentNode.parentNode.parentNode;
    var td = tr.childNodes;
    td = td[1];
    var inpt = td.childNodes;
    inpt = inpt[0];
    inpt.value = ret;
}

function count_obj(mixed_var, mode) {    // Count elements in an array, or properties in an object
    var key, cnt = 0;
    if (mode === 'COUNT_RECURSIVE') mode = 1;
    if (mode != 1) mode = 0;
    for (key in mixed_var) {
        if (typeof(mixed_var[key]) != 'function') {
            ++cnt;
        }
        if (mode == 1 && mixed_var[key] && (mixed_var[key].constructor === Array || mixed_var[key].constructor === Object)) {
            cnt += count(mixed_var[key], 1);
        }
    }
    return cnt;
}

function clone(o) {
    if (!o || 'object' !== typeof o) {
        return o;
    }
    var c = 'function' === typeof o.pop ? [] : {};
    var p, v;
    for (p in o) {
        if (o.hasOwnProperty(p)) {
            v = o[p];
            if (v && 'object' === typeof v) {
                c[p] = clone(v);
            }
            else {
                c[p] = v;
            }
        }
    }
    return c;
}

function tree2str(arr, level) {
    var print_red_text = "";
    if (!level) level = 0;
    var level_padding = "";
    for (var j = 0; j < level + 1; j++) level_padding += "&nbsp;&nbsp;&nbsp;&nbsp;";
    if (typeof(arr) == 'object') {
        for (var item in arr) {
            var value = arr[item];
            if (typeof(value) == 'object') {
                print_red_text += level_padding + "'" + item + "' :<br>";
                print_red_text += tree2str(value, level + 1);
            } else {
                print_red_text += level_padding + "'" + item + "' => \"" + value + "\"<br>";
            }
        }
    }
    else {
        print_red_text = "===>" + arr + "<===(" + typeof(arr) + ")";
    }
    return print_red_text;
}

function print_r(obj) {
    Ext.onReady(function () {
        m = new Ext.Window({
            width: 400,
            height: 400,
            autoScroll: true,
            html: tree2str(obj)
        });
        m.show();
    });
}

function mark_service(td) {
    var service = td.innerHTML;
    if (td.style.background == 'yellow') {
        var color = 'transparent';
    } else {
        var color = 'yellow';
    }
    els = getElementsByClass('service_show');
    for (el in els) {
        if (typeof(els[el]) == 'object') {
            if (els[el].className == 'service_show' && els[el].innerHTML == service) {
                els[el].style.background = color;
            }
        }
    }

}

function copy_worktime(n, form) {

    for (i = 1; i <= 7; i++) {
        document[form]['worktime_from_h[' + i + ']'].value = document[form]['worktime_from_h[' + n + ']'].value;
        document[form]['worktime_from_m[' + i + ']'].value = document[form]['worktime_from_m[' + n + ']'].value;
        document[form]['worktime_to_h[' + i + ']'].value = document[form]['worktime_to_h[' + n + ']'].value;
        document[form]['worktime_to_m[' + i + ']'].value = document[form]['worktime_to_m[' + n + ']'].value;
    }
}

function copy_worktime_division(n, form) {
    for (i = 1; i <= 7; i++) {
        document['reg_set']['worktime_' + form + '_from_h[' + i + ']'].value = document['reg_set']['worktime_' + form + '_from_h[' + n + ']'].value;
        document['reg_set']['worktime_' + form + '_from_m[' + i + ']'].value = document['reg_set']['worktime_' + form + '_from_m[' + n + ']'].value;
        document['reg_set']['worktime_' + form + '_to_h[' + i + ']'].value = document['reg_set']['worktime_' + form + '_to_h[' + n + ']'].value;
        document['reg_set']['worktime_' + form + '_to_m[' + i + ']'].value = document['reg_set']['worktime_' + form + '_to_m[' + n + ']'].value;
    }
}


function getElementsByClass(searchClass, node, tag) {
    var classElements = [];

    if (node == null) {
        node = document;
    }

    if (tag == null) {
        tag = '*';
    }

    var els = node.getElementsByTagName(tag);
    var elsLen = els.length;

    var pattern = new RegExp("(^|\\s)" + searchClass + "(\\s|$)");
    var i;
    for (i = 0; i < elsLen; i++) {
        if (pattern.test(els[i].className)) {
            classElements[classElements.length] = els[i];
        }
    }

    return classElements;
}

function select_all_col(btn, col) {
    var status = true;
    if (document.getElementById) {
        //���������� ������
        status = btn.checked;


        //���� ������� �������� ���������� ���������� ������
        tr = btn;
        while (tr.tagName != 'TR') tr = tr.parentNode;
        btns = tr.parentNode;
        btns = btns.childNodes;
        for (i = 0; i < btns.length; i++) {

            if (btns[i] != tr) {
                b = btns[i].childNodes;
                if (b[col]) {
                    b = b[col];
                    b = b.childNodes;

                    var j = 0;
                    var b1 = b[j];
                    //while(b1.tagName!="INPUT") { b1=b[j]; j++; if(j>10){break;} }
                    if (b1.tagName == "INPUT") b1.checked = status;

                }
            }
        }
    }
}

function select_all_row(btn) {
    var status = true;
    if (document.getElementById) {
        //���������� ������
        status = btn.checked;

        tr = btn;
        while (tr.tagName != 'TR') tr = tr.parentNode;
        btns = tr.childNodes;
        for (i = 0; i < btns.length; i++) {
            b = btns[i].childNodes;
            for (j = 0; j < b.length; j++) {
                if (b[j].checked === false || b[j].checked === true) {
                    b[j].checked = status;
                }
            }
        }

    }
}

function dropRow(btn) {
    if (document.getElementById) {
        tr = btn;
        while (tr.tagName != 'TR') tr = tr.parentNode;

        btns = tr.parentNode;
        btns = btns.childNodes;

        btnss = new Array();
        j = 0;
        for (i = 0; i < btns.length; i++) {
            if (btns[i] != tr) {
                btnss[j] = btns[i];
                j++;
            }
        }
        btns = btnss;
        if (btns[btns.length - 1] != null) {
            btns = btns[btns.length - 1].childNodes;

            for (i = 0; i < btns.length; i++) {
                if (
                    typeof(btns[i].style) != 'undefined'
                ) {
                    if (btns[i].style.visibility == 'visible') {
                        btns[i].style.visibility = 'hidden';
                    } else {
                        btns[i].style.visibility = 'visible';
                    }
                }
            }
            btns = btns[btns.length - 1];
            btns.style.visibility = 'visible';
            tr.parentNode.removeChild(tr);
        }
    }
}

function addRow(tr) {
    if (document.getElementById) {
        while (tr.tagName != 'TR') {
            tr = tr.parentNode;
        }
        var newTr = tr.parentNode.insertBefore(tr.cloneNode(true), tr.nextSibling);
    }
}


function create_address(type) {
    var data = new Array();
    var url = '/dialogs/address/?' + type + '=1';
    var data = window.open(url, null, "status:no;dialogWidth:400px;scroll:no;dialogHeight:400px;help:no;");
    return data;
}

function datedigit(d) {
    arr_d = d.split(".");
    return arr_d[2] * 32140800 + arr_d[1] * 2678400 + arr_d[0] * 86400;
}

function search_to_select(obj, obj_sel, txt) {
    var rep = /[\xA0]{1,4}/g;
    search_txt = obj.value.toLowerCase();
    search_len = search_txt.length;
    for (i = 0; i <= obj_sel.length - 1; i++) {
        var elem = obj_sel.options[i];
        elem_txt = elem.text.toLowerCase();
        elem_txt = elem_txt.replace(rep, "");
        txt = elem_txt.substring(0, search_len);
        if (txt == search_txt) {
            elem.selected = 'True';
            break;
        }
    }
}


function code_ru_eng(str) {
    var str_ru = new Array("�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�");
    var str_en = new Array("A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "[", "|", "]", "^", "_", "`", "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", "{", "|", "}", "~", "?", "@", "<", ">");
    var str_eng = "";
    for (z = 0; z <= str.length - 1; z++) {
        s = str.substring(z, z + 1);
        zz = 0;
        for (i = 0; i <= str_ru.length - 1; i++) {
            if (str_ru[i] == s) {
                str_eng = str_eng + str_en[i];
                zz = 1;
            }
        }
        if (zz == 0) {
            str_eng = str_eng + s;
        }
    }
    return str_eng;
}

function change_img(obj, img) {
    document.all(obj).src = img.src;
}

function code_translit(str) {
    var str_ru = new Array("�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�");
    var str_tr = new Array("a", "b", "v", "g", "d", "e", "yo", "zh", "z", "i", "j", "k", "l", "m", "n", "o", "p", "r", "s", "t", "u", "f", "kh", "ts", "ch", "sh", "sch", "", "y", "", "je", "ju", "ya", "a", "b", "v", "g", "d", "e", "yo", "zh", "z", "i", "j", "k", "l", "m", "n", "o", "p", "r", "s", "t", "u", "f", "kh", "ts", "ch", "sh", "sch", "", "y", "", "je", "ju", "ya");
    var str_tran = "";
    for (z = 0; z <= str.length - 1; z++) {
        s = str.substring(z, z + 1);
        zz = 0;
        for (i = 0; i <= str_ru.length - 1; i++) {
            if (str_ru[i] == s) {
                str_tran = str_tran + str_tr[i];
                zz = 1;
            }
        }
        if (zz == 0) {
            str_tran = str_tran + s;
        }
    }
    return str_tran;
}

function code_translit_kb(str, t) {
    // t=0 - ru -> eng
    // t=1 - eng -> ru

    var str1 = new Array("�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�");
    var str2 = new Array("F", "<", "D", "U", "L", "T", "~", ":", "P", "B", "Q", "R", "K", "V", "Y", "J", "G", "H", "C", "N", "E", "A", "{", "W", "X", "I", "O", "}", "S", "M", "", ">", "Z", "f", ",", "d", "u", "l", "t", "`", ";", "p", "b", "q", "r", "k", "v", "y", "j", "g", "h", "c", "n", "e", "a", "[", "w", "x", "i", "o", "]", "s", "m", "", ".", "z");
    var ret = "";
    var str_ru;
    var str_en;

    if (t == 0) {
        str_ru = str1;
        str_en = str2;
    }
    else {
        str_ru = str2;
        str_en = str1;
    }

    for (z = 0; z <= str.length - 1; z++) {
        s = str.substring(z, z + 1);
        zz = 0;
        for (i = 0; i <= str_ru.length - 1; i++) {
            if (str_ru[i] == s) {
                ret = ret + str_en[i];
                zz = 1;
            }
        }
        if (zz == 0) {
            ret = ret + s;
        }
    }
    return ret;
}

function showtimeout() {
    user_timeout = user_timeout - 1;
    m = parseInt((user_timeout / 60), 10);
    s = (user_timeout - parseInt((user_timeout / 60), 10) * 60);
    if (s < 10) {
        s = 0 + '' + s;
    }
    if (m > 0) {
        m = m + ' ���. ';
    } else {
        m = '';
    }
    txt = m + s + ' c��.';
    if (user_timeout >= 0 && user_timeout <= 10) {
        txt = '<font color="#ff0000">' + user_timeout + ' c��.</font>';
    }
    if (user_timeout < 0) {
        document.all('timeout_set').innerHTML = '';
        document.all('timeout_block').innerHTML = '<font color="#ff0000">�������� �������� "�������"</font>';
    } else {
        document.all('timeout_set').innerHTML = txt;
    }
    if (user_timeout >= 0) {
        starttimer("showtimeout()", 1000);
    }
}
function starttimer(comm, time) {
    setTimeout(comm, time);
}

function get_cookie(param) {
    var label = param + '=';
    var labelLen = label.length;
    var cLen = document.cookie.length;
    var i = 0;
    while (i < cLen) {
        var j = i + labelLen;
        if (document.cookie.substring(i, j) == label) {
            var cEnd = document.cookie.indexOf(';', j);
            if (cEnd == -1) {
                cEnd = document.cookie.length
            }
            return unescape(document.cookie.substring(j, cEnd));
        }
        i++
    }
    return '';
}

function set_cookie(param, str, sess) {
    if (typeof(sess) == 'undefined') {
        var expire = new Date();
        expire.setTime(expire.getTime() + ( 7 * 24 * 60 * 60 * 1000 ));
        document.cookie = param + "=" + escape(str) + ";path=/;expires=" + expire.toGMTString();
    } else {
        document.cookie = param + "=" + escape(str) + ";path=/;";
    }

}

function dropdown_menu_hack(el) {
    if (el.runtimeStyle.behavior.toLowerCase() == "none") {
        return;
    }
    el.runtimeStyle.behavior = "none";
    //var ie5 = (document.namespaces==null);
    var ie5 = 1;

    el.ondblclick = function (e) {
        window.event.returnValue = false;
        return false;
    }

    if (window.createPopup == null) {

        var fid = "dropdown_menu_hack_" + Date.parse(new Date());

        window.createPopup = function () {
            if (window.createPopup.frameWindow == null) {
                el.insertAdjacentHTML("AfterEnd", "<iframe   id='" + fid + "' name='" + fid + "' src='about:blank'  frameborder='1' scrolling='no'></></iframe>");
                var f = document.frames[fid];
                f.document.open();
                f.document.write("<html><body></body></html>");
                f.document.close();
                f.fid = fid;


                var fwin = document.getElementById(fid);
                fwin.style.cssText = "position:absolute;top:0;left:0;display:none;z-index:99999;";


                f.show = function (px, py, pw, ph, baseElement) {
                    py = py + baseElement.getBoundingClientRect().top + Math.max(document.body.scrollTop, document.documentElement.scrollTop);
                    px = px + baseElement.getBoundingClientRect().left + Math.max(document.body.scrollLeft, document.documentElement.scrollLeft);
                    fwin.style.width = pw + "px";
                    fwin.style.height = ph + "px";
                    fwin.style.posLeft = px;
                    fwin.style.posTop = py;
                    fwin.style.display = "block";
                }


                f_hide = function (e) {
                    if (window.event && window.event.srcElement && window.event.srcElement.tagName && window.event.srcElement.tagName.toLowerCase() == "select") {
                        return true;
                    }
                    fwin.style.display = "none";
                }
                f.hide = f_hide;
                document.attachEvent("onclick", f_hide);
                document.attachEvent("onkeydown", f_hide);

            }
            return f;
        }
    }

    function showMenu() {

        function selectMenu(obj) {
            var o = document.createElement("option");
            o.value = obj.value;
            o.innerHTML = obj.innerHTML;
            while (el.options.length > 0) {
                el.options[0].removeNode(true);
            }
            el.appendChild(o);
            el.title = o.innerHTML;
            el.contentIndex = obj.selectedIndex;
            el.menu.hide();
        }


        el.menu.show(0, el.offsetHeight, 10, 10, el);
        var mb = el.menu.document.body;

        mb.style.cssText = "border:solid 1px black;margin:0;padding:0;overflow-y:auto;overflow-x:auto;background:white;text-aligbn:center;font-family:Verdana;font-size:12px;";
        var t = el.contentHTML;
        t = t.replace(/<select/gi, '<ul');
        t = t.replace(/<option/gi, '<li');
        t = t.replace(/<\/option/gi, '</li');
        t = t.replace(/<\/select/gi, '</ul');

        t = t.replace(/<optgroup label="([^"]*)">/gi, '<b><i>$1</i></b><br>');
        t = t.replace(/<\/optgroup>/gi, '');

        //alert(t);
        mb.innerHTML = t;


        el.select = mb.all.tags("ul")[0];
        el.select.style.cssText = "list-style:none;margin:0;padding:5px;";
        mb.options = el.select.getElementsByTagName("li");

        for (var i = 0; i < mb.options.length; i++) {
            mb.options[i].selectedIndex = i;
            mb.options[i].style.cssText = "list-style:none;margin:0;padding:1px 8px 3px 8px;width/**/:100%;cursor:hand;cursor:pointer;white-space:nowrap;"
            mb.options[i].title = mb.options[i].innerHTML;
            mb.options[i].innerHTML = "<nobr>" + mb.options[i].innerHTML + "</nobr>";
            mb.options[i].onmouseover = function () {
                if (mb.options.selected) {
                    mb.options.selected.style.background = "white";
                    mb.options.selected.style.color = "black";
                }
                mb.options.selected = this;
                this.style.background = "#2A3474";
                this.style.color = "white";
            }

            mb.options[i].onmouseout = function () {
                this.style.background = "white";
                this.style.color = "black";
            }
            mb.options[i].onmousedown = function () {
                selectMenu(this);
            }
            mb.options[i].onkeydown = function () {
                selectMenu(this);
            }


            if (i == el.contentIndex) {
                mb.options[i].style.background = "#333366";
                mb.options[i].style.color = "white";
                mb.options.selected = mb.options[i];
            }
        }


        var mw = Math.max(( el.select.offsetWidth + 22 ), el.offsetWidth + 22);
        mw = Math.max(mw, ( mb.scrollWidth + 22));
        var mh = mb.options.length * 15 + 8;

        var mx = (ie5) ? -3 : 0;
        var my = el.offsetHeight - 2;
        var docH = document.documentElement.offsetHeight;
        var bottomH = docH - el.getBoundingClientRect().bottom;

        mh = Math.min(mh, Math.max(( docH - el.getBoundingClientRect().top - 50), 100));

        if (( bottomH < mh)) {

            mh = Math.max((bottomH - 12), 10);
            if (mh < 100) {
                my = -100;

            }
            mh = Math.max(mh, 100);
        }


        self.focus();

        el.menu.show(mx, my, mw, mh, el);
        sync = null;
        if (mb.options.selected) {
            mb.scrollTop = mb.options.selected.offsetTop;
        }


        window.onresize = function () {
            el.menu.hide()
        };
    }

    function switchMenu() {
        if (event.keyCode) {
            if (event.keyCode == 40) {
                el.contentIndex++;
            }
            else if (event.keyCode == 38) {
                el.contentIndex--;
            }
        }
        else if (event.wheelDelta) {
            if (event.wheelDelta >= 120)
                el.contentIndex++;
            else if (event.wheelDelta <= -120)
                el.contentIndex--;
        } else {
            return true;
        }


        if (el.contentIndex > (el.contentOptions.length - 1)) {
            el.contentIndex = 0;
        }
        else if (el.contentIndex < 0) {
            el.contentIndex = el.contentOptions.length - 1;
        }

        var o = document.createElement("option");
        o.value = el.contentOptions[el.contentIndex].value;
        o.innerHTML = el.contentOptions[el.contentIndex].text;
        while (el.options.length > 0) {
            el.options[0].removeNode(true);
        }
        el.appendChild(o);
        el.title = o.innerHTML;
    }

    if (dropdown_menu_hack.menu == null) {
        dropdown_menu_hack.menu = window.createPopup();
        document.attachEvent("onkeydown", dropdown_menu_hack.menu.hide);
    }
    el.menu = dropdown_menu_hack.menu;
    el.contentOptions = new Array();
    el.contentIndex = el.selectedIndex;
    el.contentHTML = el.outerHTML;

    for (var i = 0; i < el.options.length; i++) {
        el.contentOptions [el.contentOptions.length] =
        {
            "value": el.options[i].value,
            "text": el.options[i].innerHTML
        }

        if (!el.options[i].selected) {
            el.options[i].removeNode(true);
            i--;
        }
        ;
    }


    el.onkeydown = switchMenu;
    el.onclick = showMenu;
    el.onmousewheel = switchMenu;

}


// ~~~ MD5.js ~~~
function array(n) {
    for (i = 0; i < n; i++) this[i] = 0;
    this.length = n;
}

function integer(n) {
    return n % (0xffffffff + 1);
}

function shr(a, b) {
    a = integer(a);
    b = integer(b);
    if (a - 0x80000000 >= 0) {
        a = a % 0x80000000;
        a >>= b;
        a += 0x40000000 >> (b - 1);
    } else
        a >>= b;
    return a;
}

function shl1(a) {
    a = a % 0x80000000;
    if (a & 0x40000000 == 0x40000000) {
        a -= 0x40000000;
        a *= 2;
        a += 0x80000000;
    } else
        a *= 2;
    return a;
}

function shl(a, b) {
    a = integer(a);
    b = integer(b);
    for (var i = 0; i < b; i++) a = shl1(a);
    return a;
}

function and(a, b) {
    a = integer(a);
    b = integer(b);
    var t1 = (a - 0x80000000);
    var t2 = (b - 0x80000000);
    if (t1 >= 0)
        if (t2 >= 0)
            return ((t1 & t2) + 0x80000000);
        else
            return (t1 & b);
    else if (t2 >= 0)
        return (a & t2);
    else
        return (a & b);
}

function or(a, b) {
    a = integer(a);
    b = integer(b);
    var t1 = (a - 0x80000000);
    var t2 = (b - 0x80000000);
    if (t1 >= 0)
        if (t2 >= 0)
            return ((t1 | t2) + 0x80000000);
        else
            return ((t1 | b) + 0x80000000);
    else if (t2 >= 0)
        return ((a | t2) + 0x80000000);
    else
        return (a | b);
}

function xor(a, b) {
    a = integer(a);
    b = integer(b);
    var t1 = (a - 0x80000000);
    var t2 = (b - 0x80000000);
    if (t1 >= 0)
        if (t2 >= 0)
            return (t1 ^ t2);
        else
            return ((t1 ^ b) + 0x80000000);
    else if (t2 >= 0)
        return ((a ^ t2) + 0x80000000);
    else
        return (a ^ b);
}

function not(a) {
    a = integer(a);
    return (0xffffffff - a);
}

/* Here begin the real algorithm */

var state = new array(4);
var count = new array(2);
count[0] = 0;
count[1] = 0;
var buffer = new array(64);
var transformBuffer = new array(16);
var digestBits = new array(16);

var S11 = 7;
var S12 = 12;
var S13 = 17;
var S14 = 22;
var S21 = 5;
var S22 = 9;
var S23 = 14;
var S24 = 20;
var S31 = 4;
var S32 = 11;
var S33 = 16;
var S34 = 23;
var S41 = 6;
var S42 = 10;
var S43 = 15;
var S44 = 21;

function F(x, y, z) {
    return or(and(x, y), and(not(x), z));
}

function G(x, y, z) {
    return or(and(x, z), and(y, not(z)));
}

function H(x, y, z) {
    return xor(xor(x, y), z);
}

function I(x, y, z) {
    return xor(y, or(x, not(z)));
}

function rotateLeft(a, n) {
    return or(shl(a, n), (shr(a, (32 - n))));
}

function FF(a, b, c, d, x, s, ac) {
    a = a + F(b, c, d) + x + ac;
    a = rotateLeft(a, s);
    a = a + b;
    return a;
}

function GG(a, b, c, d, x, s, ac) {
    a = a + G(b, c, d) + x + ac;
    a = rotateLeft(a, s);
    a = a + b;
    return a;
}

function HH(a, b, c, d, x, s, ac) {
    a = a + H(b, c, d) + x + ac;
    a = rotateLeft(a, s);
    a = a + b;
    return a;
}

function II(a, b, c, d, x, s, ac) {
    a = a + I(b, c, d) + x + ac;
    a = rotateLeft(a, s);
    a = a + b;
    return a;
}

function transform(buf, offset) {
    var a = 0, b = 0, c = 0, d = 0;
    var x = transformBuffer;

    a = state[0];
    b = state[1];
    c = state[2];
    d = state[3];

    for (i = 0; i < 16; i++) {
        x[i] = and(buf[i * 4 + offset], 0xff);
        for (j = 1; j < 4; j++) {
            x[i] += shl(and(buf[i * 4 + j + offset], 0xff), j * 8);
        }
    }

    /* Round 1 */
    a = FF(a, b, c, d, x[0], S11, 0xd76aa478);
    /* 1 */
    d = FF(d, a, b, c, x[1], S12, 0xe8c7b756);
    /* 2 */
    c = FF(c, d, a, b, x[2], S13, 0x242070db);
    /* 3 */
    b = FF(b, c, d, a, x[3], S14, 0xc1bdceee);
    /* 4 */
    a = FF(a, b, c, d, x[4], S11, 0xf57c0faf);
    /* 5 */
    d = FF(d, a, b, c, x[5], S12, 0x4787c62a);
    /* 6 */
    c = FF(c, d, a, b, x[6], S13, 0xa8304613);
    /* 7 */
    b = FF(b, c, d, a, x[7], S14, 0xfd469501);
    /* 8 */
    a = FF(a, b, c, d, x[8], S11, 0x698098d8);
    /* 9 */
    d = FF(d, a, b, c, x[9], S12, 0x8b44f7af);
    /* 10 */
    c = FF(c, d, a, b, x[10], S13, 0xffff5bb1);
    /* 11 */
    b = FF(b, c, d, a, x[11], S14, 0x895cd7be);
    /* 12 */
    a = FF(a, b, c, d, x[12], S11, 0x6b901122);
    /* 13 */
    d = FF(d, a, b, c, x[13], S12, 0xfd987193);
    /* 14 */
    c = FF(c, d, a, b, x[14], S13, 0xa679438e);
    /* 15 */
    b = FF(b, c, d, a, x[15], S14, 0x49b40821);
    /* 16 */

    /* Round 2 */
    a = GG(a, b, c, d, x[1], S21, 0xf61e2562);
    /* 17 */
    d = GG(d, a, b, c, x[6], S22, 0xc040b340);
    /* 18 */
    c = GG(c, d, a, b, x[11], S23, 0x265e5a51);
    /* 19 */
    b = GG(b, c, d, a, x[0], S24, 0xe9b6c7aa);
    /* 20 */
    a = GG(a, b, c, d, x[5], S21, 0xd62f105d);
    /* 21 */
    d = GG(d, a, b, c, x[10], S22, 0x2441453);
    /* 22 */
    c = GG(c, d, a, b, x[15], S23, 0xd8a1e681);
    /* 23 */
    b = GG(b, c, d, a, x[4], S24, 0xe7d3fbc8);
    /* 24 */
    a = GG(a, b, c, d, x[9], S21, 0x21e1cde6);
    /* 25 */
    d = GG(d, a, b, c, x[14], S22, 0xc33707d6);
    /* 26 */
    c = GG(c, d, a, b, x[3], S23, 0xf4d50d87);
    /* 27 */
    b = GG(b, c, d, a, x[8], S24, 0x455a14ed);
    /* 28 */
    a = GG(a, b, c, d, x[13], S21, 0xa9e3e905);
    /* 29 */
    d = GG(d, a, b, c, x[2], S22, 0xfcefa3f8);
    /* 30 */
    c = GG(c, d, a, b, x[7], S23, 0x676f02d9);
    /* 31 */
    b = GG(b, c, d, a, x[12], S24, 0x8d2a4c8a);
    /* 32 */

    /* Round 3 */
    a = HH(a, b, c, d, x[5], S31, 0xfffa3942);
    /* 33 */
    d = HH(d, a, b, c, x[8], S32, 0x8771f681);
    /* 34 */
    c = HH(c, d, a, b, x[11], S33, 0x6d9d6122);
    /* 35 */
    b = HH(b, c, d, a, x[14], S34, 0xfde5380c);
    /* 36 */
    a = HH(a, b, c, d, x[1], S31, 0xa4beea44);
    /* 37 */
    d = HH(d, a, b, c, x[4], S32, 0x4bdecfa9);
    /* 38 */
    c = HH(c, d, a, b, x[7], S33, 0xf6bb4b60);
    /* 39 */
    b = HH(b, c, d, a, x[10], S34, 0xbebfbc70);
    /* 40 */
    a = HH(a, b, c, d, x[13], S31, 0x289b7ec6);
    /* 41 */
    d = HH(d, a, b, c, x[0], S32, 0xeaa127fa);
    /* 42 */
    c = HH(c, d, a, b, x[3], S33, 0xd4ef3085);
    /* 43 */
    b = HH(b, c, d, a, x[6], S34, 0x4881d05);
    /* 44 */
    a = HH(a, b, c, d, x[9], S31, 0xd9d4d039);
    /* 45 */
    d = HH(d, a, b, c, x[12], S32, 0xe6db99e5);
    /* 46 */
    c = HH(c, d, a, b, x[15], S33, 0x1fa27cf8);
    /* 47 */
    b = HH(b, c, d, a, x[2], S34, 0xc4ac5665);
    /* 48 */

    /* Round 4 */
    a = II(a, b, c, d, x[0], S41, 0xf4292244);
    /* 49 */
    d = II(d, a, b, c, x[7], S42, 0x432aff97);
    /* 50 */
    c = II(c, d, a, b, x[14], S43, 0xab9423a7);
    /* 51 */
    b = II(b, c, d, a, x[5], S44, 0xfc93a039);
    /* 52 */
    a = II(a, b, c, d, x[12], S41, 0x655b59c3);
    /* 53 */
    d = II(d, a, b, c, x[3], S42, 0x8f0ccc92);
    /* 54 */
    c = II(c, d, a, b, x[10], S43, 0xffeff47d);
    /* 55 */
    b = II(b, c, d, a, x[1], S44, 0x85845dd1);
    /* 56 */
    a = II(a, b, c, d, x[8], S41, 0x6fa87e4f);
    /* 57 */
    d = II(d, a, b, c, x[15], S42, 0xfe2ce6e0);
    /* 58 */
    c = II(c, d, a, b, x[6], S43, 0xa3014314);
    /* 59 */
    b = II(b, c, d, a, x[13], S44, 0x4e0811a1);
    /* 60 */
    a = II(a, b, c, d, x[4], S41, 0xf7537e82);
    /* 61 */
    d = II(d, a, b, c, x[11], S42, 0xbd3af235);
    /* 62 */
    c = II(c, d, a, b, x[2], S43, 0x2ad7d2bb);
    /* 63 */
    b = II(b, c, d, a, x[9], S44, 0xeb86d391);
    /* 64 */

    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;

}

function init() {
    count[0] = count[1] = 0;
    state[0] = 0x67452301;
    state[1] = 0xefcdab89;
    state[2] = 0x98badcfe;
    state[3] = 0x10325476;
    for (i = 0; i < digestBits.length; i++)
        digestBits[i] = 0;
}

function update(b) {
    var index, i;

    index = and(shr(count[0], 3), 0x3f);
    if (count[0] < 0xffffffff - 7)
        count[0] += 8;
    else {
        count[1]++;
        count[0] -= 0xffffffff + 1;
        count[0] += 8;
    }
    buffer[index] = and(b, 0xff);
    if (index >= 63) {
        transform(buffer, 0);
    }
}

function finish() {
    var bits = new array(8);
    var padding;
    var i = 0, index = 0, padLen = 0;

    for (i = 0; i < 4; i++) {
        bits[i] = and(shr(count[0], (i * 8)), 0xff);
    }
    for (i = 0; i < 4; i++) {
        bits[i + 4] = and(shr(count[1], (i * 8)), 0xff);
    }
    index = and(shr(count[0], 3), 0x3f);
    padLen = (index < 56) ? (56 - index) : (120 - index);
    padding = new array(64);
    padding[0] = 0x80;
    for (i = 0; i < padLen; i++)
        update(padding[i]);
    for (i = 0; i < 8; i++)
        update(bits[i]);

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            digestBits[i * 4 + j] = and(shr(state[i], (j * 8)), 0xff);
        }
    }
}

/* End of the MD5 algorithm */

function hexa(n) {
    var hexa_h = "0123456789abcdef";
    var hexa_c = "";
    var hexa_m = n;
    for (hexa_i = 0; hexa_i < 8; hexa_i++) {
        hexa_c = hexa_h.charAt(Math.abs(hexa_m) % 16) + hexa_c;
        hexa_m = Math.floor(hexa_m / 16);
    }
    return hexa_c;
}


var ascii = "01234567890123456789012345678901" +
    " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ" +
    "[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";

function MD5() {
    var l, s, k, ka, kb, kc, kd;

    entree = arguments[0]
    for (var i = 1; i < arguments.length; i++) {
        entree += ":" + arguments[i]
    }

    init();
    for (k = 0; k < entree.length; k++) {
        l = entree.charAt(k);
        update(ascii.lastIndexOf(l));
    }
    finish();
    ka = kb = kc = kd = 0;
    for (i = 0; i < 4; i++) ka += shl(digestBits[15 - i], (i * 8));
    for (i = 4; i < 8; i++) kb += shl(digestBits[15 - i], ((i - 4) * 8));
    for (i = 8; i < 12; i++) kc += shl(digestBits[15 - i], ((i - 8) * 8));
    for (i = 12; i < 16; i++) kd += shl(digestBits[15 - i], ((i - 12) * 8));
    s = hexa(kd) + hexa(kc) + hexa(kb) + hexa(ka);
    /*  alert(s); */
    return s;
}
// ~~~ end MD5.js ~~~

function urlRusLat(str) {
    //str = str.toLowerCase(); // ��� � ������ �������
    var cyr2latChars = new Array(
        ['а', 'a'], ['б', 'b'], ['в', 'v'], ['г', 'g'],
        ['д', 'd'], ['е', 'e'], ['ё', 'yo'], ['ж', 'zh'], ['з', 'z'],
        ['и', 'i'], ['й', 'y'], ['к', 'k'], ['л', 'l'],
        ['м', 'm'], ['н', 'n'], ['о', 'o'], ['п', 'p'], ['р', 'r'],
        ['с', 's'], ['т', 't'], ['у', 'u'], ['ф', 'f'],
        ['х', 'h'], ['ч', 'c'], ['ц', 'ch'], ['ш', 'sh'], ['щ', 'shch'],
        ['ь', ''], ['ы', 'y'], ['ъ', ''], ['э', 'e'], ['ю', 'yu'], ['я', 'ya'],

        ['А', 'A'], ['Б', 'B'], ['В', 'V'], ['Г', 'G'],
        ['Д', 'D'], ['Е', 'E'], ['Ё', 'YO'], ['Ж', 'ZH'], ['З', 'Z'],
        ['И', 'I'], ['Й', 'Y'], ['К', 'K'], ['Л', 'L'],
        ['М', 'M'], ['Н', 'N'], ['О', 'O'], ['П', 'P'], ['Р', 'R'],
        ['С', 'S'], ['Т', 'T'], ['У', 'U'], ['Ф', 'F'],
        ['Х', 'H'], ['Ч', 'C'], ['Ц', 'CH'], ['Ш', 'SH'], ['Щ', 'SHCH'],
        ['Ь', ''], ['Ы', 'Y'], ['Ъ', ''], ['Э', 'E'], ['Ю', 'YU'], ['Я', 'YA'],

        ['a', 'a'], ['b', 'b'], ['c', 'c'], ['d', 'd'], ['e', 'e'],
        ['f', 'f'], ['g', 'g'], ['h', 'h'], ['i', 'i'], ['j', 'j'],
        ['k', 'k'], ['l', 'l'], ['m', 'm'], ['n', 'n'], ['o', 'o'],
        ['p', 'p'], ['q', 'q'], ['r', 'r'], ['s', 's'], ['t', 't'],
        ['u', 'u'], ['v', 'v'], ['w', 'w'], ['x', 'x'], ['y', 'y'],
        ['z', 'z'],

        ['A', 'A'], ['B', 'B'], ['C', 'C'], ['D', 'D'], ['E', 'E'],
        ['F', 'F'], ['G', 'G'], ['H', 'H'], ['I', 'I'], ['J', 'J'], ['K', 'K'],
        ['L', 'L'], ['M', 'M'], ['N', 'N'], ['O', 'O'], ['P', 'P'],
        ['Q', 'Q'], ['R', 'R'], ['S', 'S'], ['T', 'T'], ['U', 'U'], ['V', 'V'],
        ['W', 'W'], ['X', 'X'], ['Y', 'Y'], ['Z', 'Z'],

        ['0', '0'], ['1', '1'], ['2', '2'], ['3', '3'],
        ['4', '4'], ['5', '5'], ['6', '6'], ['7', '7'], ['8', '8'], ['9', '9'],
        ['-', '-']
    );

    var newStr = new String();

    for (var i = 0; i < str.length; i++) {

        ch = str.charAt(i);
        var newCh = '';

        for (var j = 0; j < cyr2latChars.length; j++) {
            if (ch == cyr2latChars[j][0]) {
                newCh = cyr2latChars[j][1];

            }
        }
        // ���� ������� ����������, �� ����������� ������������, ���� ��� - ������ ������
        newStr += newCh;

    }
    // ������� ����������� ����� - ������ �� ��� ���������� �������.
    // ��� �� ������� ������� �������� ������, �� ��� �������� ��� ������
    return newStr.replace(/[_]{2,}/gim, '_').replace(/\n/gim, '');
}
function to_money(money) {

    money.value = money.value.replace(/(\d{1,3})(?=((\d{3})*([^\d]|$)))/g, " $1 ");


}

function str_to_date(str) {

    return $.format.date(Date.parse(str), "dd.MM.yyyy");
}
$.fn.breakdown = function (seporator) {
    $(this).keyup(function () {
        var seporator = " ";
        var regular = /(\d{1,3}(?=(\d{3})+(?:\.\d|\b)))/g;
        var newval = $(this).val().replace(" ", "");
        newval = newval.replace(regular, '$1' + seporator);
        $(this).val(newval);
    });
    $(this).trigger("keyup");


    /*
     if(typeof $(this).val() == 'object'){
     return this.each(function(){
     num = $(this).text();
     $(this).text(make());
     });
     }

     else if(typeof $(this).val() == 'number'){
     num = num.toString();
     return make();
     }
     })*/

}