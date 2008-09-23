$(document).ready(function() {
   $("#connexion-box").load("request/connect.ajax.php");
});

function loadConnexionBox ()
{
        $("#connexion-box").load("request/connect.ajax.php?mode=connect");
}

function doDisconnexion()
{
     // send request
     $.post("request/connect.ajax.php", {logout: ''}, function(data) {
       $("#connexion-box").html(data); });
     location.href="index.php";
}

function doRegister()
{
     // send request
     if($('#frm_pass1').val() != $('#frm_pass2').val())
     	alert("Vos mots de passe ne sont pas identiques");
     else
       $.post("request/register.ajax.php", {login: $('#frm_login').val(), pass: $('#frm_pass1').val(), bunny: $('#frm_name').val(), serial: $('#frm_serial').val()}, function(data) {
         $("#inscription-box").html(data); });
}

function doConnexion()
{
     // send request
     $.post("request/connect.ajax.php", {login: $('#login').val(), pass: $('#pass').val()}, function(data) {
       $("#connexion-box").html(data); });
     location.href="index.php";
}

function changePluginStatus(plugin, statut)
{
     // send request
     $.post("request/pluginServer.ajax.php", {plug: plugin, stat: statut}, function(data) {
       $("#tablePluginServer").html(data); });
}

