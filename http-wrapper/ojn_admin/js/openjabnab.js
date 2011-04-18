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

function doAssociate()
{
       $.post("request/addbunny.ajax.php", {login: $('#frm_login').val(), serial: $('#frm_serial').val()}, function(data) {
         $("#association-box").html(data); });
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
     $.post("request/connect.ajax.php", {login: $('#login').val(), pass: $('#pass').val(), username: $('#login').val()}, function(data) {
       $("#connexion-box").html(data); });
//     location.href="index.php";
}

function reloadSystemPlugin(plugin)
{
     // send request
     $.post("request/pluginSystemServer.ajax.php", {plug: plugin, reload: true}, function(data) {
       $("#tableSystemPluginServer").html(data); });
}

function reloadBunnyPlugin(plugin)
{
     // send request
     $.post("request/pluginBunnyServer.ajax.php", {plug: plugin, reload: true}, function(data) {
       $("#tableBunnyPluginServer").html(data); });
}

function changeSystemPluginStatus(plugin, statut)
{
     // send request
     $.post("request/pluginSystemServer.ajax.php", {plug: plugin, stat: statut}, function(data) {
       $("#tableSystemPluginServer").html(data); });
}

function changeBunnyPluginStatus(plugin, statut)
{
     // send request
     $.post("request/pluginBunnyServer.ajax.php", {plug: plugin, stat: statut}, function(data) {
       $("#tableBunnyPluginServer").html(data); });
}
function registerBunnyForPlugin(plugin, statut)
{
     // send request
     $.post("request/pluginBunny.ajax.php", {plug: plugin, stat: statut}, function(data) {
       $("#tablePluginBunny").html(data); });
}

function setupPlugin(plugin)
{
        $("#setupPlugin").load("request/pluginGeneralSetup.ajax.php?plugin="+plugin);
}

function setupPluginForBunny(plugin)
{
        $("#setupPluginBunny").load("request/pluginSetup.ajax.php?plugin="+plugin);
}
