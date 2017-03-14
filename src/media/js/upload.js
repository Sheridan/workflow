
function initUpload(id, uploadURL)
{
    var uploader = $("#"+id);
    uploader.plupload
       ({
            runtimes      : 'html5,flash,silverlight,html4',
            url           : uploadURL,
            max_file_size : '4Gb',
            rename        : true,
            sortable      : true,
            sortable      : true,
            views: {
                        list  : true,
                        thumbs: true,
                        active: 'thumbs'
                    },
            flash_swf_url      : '/media/js/pupload/Moxie.swf',
            silverlight_xap_url: '/media/js/pupload/Moxie.xap'
        });
}
