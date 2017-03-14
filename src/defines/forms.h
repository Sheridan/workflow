#ifndef FORMS_H
#define FORMS_H
#include <cppcms/form.h>

#define CMS_RENDER_FORM(_form_name) \
  cppcms::form_context fcxt##_form_name(response().out(), cppcms::form_flags::as_xhtml, cppcms::form_flags::as_space); \
  response().out() << "<form method='POST' action='/auth'>"; \
  _form_name.render(fcxt##_form_name); \
  response().out() << "</form>";



#endif // FORMS_H
