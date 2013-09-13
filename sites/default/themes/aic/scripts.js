$(document).ready(function() {

// find last p tag in teasers and set to inline to move read more into teaser properly.
		$('.teaser').find('p:last').each(function(i) {
		    $(this).addClass('inline');
		});	

});