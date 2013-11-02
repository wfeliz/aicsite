$(document).ready(function() {

// find last p tag in teasers and set to inline to move read more into teaser properly.
		$('.teaser').find('p:last').each(function(i) {
		    $(this).addClass('inline');
		});	

});
var is_touch_device = 'ontouchstart' in document.documentElement;
Drupal.behaviors.fixTopBarMobile = function(context){
  var $body = $('body');
  if (is_touch_device) {
    $('input, select')
      .focus(function(e) {
        $body.addClass('fixfixed');
      })
    .blur(function(e) {
        $body.removeClass('fixfixed');
    });
  }
};