<?php

/**
 * @todo Write file documentation.
 */

/**
 * Pay.nl Payment settings.
 */
function paynl_payment_settings_form($form, &$form_state) {
  $form['paynl_payment_test_message'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show message on all pages if Pay.nl is in test mode.'),
    '#default_value' => variable_get('paynl_payment_test_message', TRUE),
  );

  return system_settings_form($form);
}
