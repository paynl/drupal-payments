<?php

/**
 * @todo Write file documentation.
 */

/**
 * Pay.nl payment methods overview.
 */
function paynl_payment_methods_overview() {
  // Get payment methods based on the PaynlPaymentMethodController.
  $payment_methods = entity_load('payment_method', FALSE,
    array('controller_class_name' => 'PaynlPaymentMethodController'));
  dpm($payment_methods);

  $header = array(t('PID'), t('Name'), t('Machine name'), t('Operations'));
  $rows = array();
  foreach ($payment_methods as $payment_method) {
    $row = array(
      $payment_method->pmid,
      $payment_method->title_specific,
      $payment_method->name,
      l(t('settings'), 'admin/config/services/payment/paynl/method/' . $payment_method->pmid),
    );

    $rows[] = $row;
  }

  return theme('table', array('header' => $header, 'rows' => $rows));
}

/**
 * Pay.nl payment method settings.
 */
function paynl_payment_method_settings_form($form, &$form_state, $pmid) {
  // Get configuration data from controller.
  $payment_methods = entity_load('payment_method', FALSE,
    array('controller_class_name' => 'PaynlPaymentMethodController'));
  $payment_method = $payment_methods[$pmid];
  $controller_data = $payment_method->controller_data;

  // Set page title.
  drupal_set_title(t('@title settings', array('@title' => $payment_method->title_specific)));

  // Configure API.
  $api_key = $controller_data['paynl_id'];
  $service_id = $controller_data['paynl_service_id'];
  \Paynl\Config::setApiToken($api_key);
  \Paynl\Config::setServiceId($service_id);

  // Get the Pay.nl payment methods.
  $methods = \Paynl\Paymentmethods::getList();

  $form['payment_methods'] = array();
  foreach ($methods as $id => $method) {
    $form['payment_methods'][$id] = array(
      'enabled' => array(
        '#type' => 'checkbox',
        '#title' => t('Enabled'),
        '#return_value' => 1,
      ),
      'id' => array(
        '#type' => 'value',
        '#title' => t('ID'),
        '#value' => $method['id'],
      ),
      'name' => array(
        '#type' => 'value',
        '#title' => t('Name'),
        '#value' => $method['name'],
      ),
      'fee' => array(
        '#type' => 'textfield',
        '#title' => t('Transaction fee'),
        '#default_value' => '0',
        '#size' => 8,
      ),
      'fee_type' => array(
        '#type' => 'select',
        '#title' => t('Transaction fee type'),
        '#options' => array('amount' => 'amount', 'percentage' => 'percentage'),
        '#default_value' => 'amount',
      ),
      'countries' => array(
        '#type' => 'select',
        '#multiple' => TRUE,
        '#title' => t('Available for'),
        '#options' => country_get_list(),
      ),
      'min' => array(
        '#type' => 'textfield',
        '#title' => t('Minimal order amount'),
        '#default_value' => '0',
        '#size' => 8,
      ),
      'max' => array(
        '#type' => 'textfield',
        '#title' => t('Maximum order amount'),
        '#description' => t('Use -1 if there is no maximum amount for this method'),
        '#default_value' => '-1',
        '#size' => 8,
      ),
      'weight' => array(
        '#type' => 'weight',
        '#title' => t('Weight'),
        '#default_value' => 0,
        '#delta' => 10,
        '#title_display' => 'invisible',
      ),
    );
  }

  $form['actions'] = array(
    'submit' => array(
      '#type' => 'submit',
      '#value' => t('Submit'),
    ),
  );

  $form['#theme'] = 'paynl_payment_method_settings_form';

  return $form;
}
